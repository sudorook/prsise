//
//  plink.hpp
//  plink
//
//  Created by Shing Wan Choi on 18/08/2016.
//  Copyright © 2016 Shing Wan Choi. All rights reserved.
//

#ifndef plink_hpp
#define plink_hpp

#include <stdio.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <deque>
#include <cstring>
#include <map>
#include <limits.h>
#include <vector>
#include <emmintrin.h>
#include <assert.h>
#include "misc.hpp"
#include "snp.hpp"

class PLINK{
public:
    //Initialize plink object with the bim bed fam file prefix
    PLINK(std::string prefix, size_t thread=1):m_prefix(prefix),m_thread(thread){
        m_init = false;
        m_bit_size = sizeof(long_type)*CHAR_BIT;
        m_num_bytes=0;
        m_num_sample=0;
        m_num_snp=0;
        m_required_bit = 0;
        m_snp_iter=0;
    };
    ~PLINK();
    void initialize();
    void initialize(std::map<std::string, bool> &inclusion, std::vector<SNP> &snp_list, const std::map<std::string, size_t> &snp_index);
    int read_snp(int num_snp, bool ld=false);
    void lerase(int num);
    size_t get_num_snp() const{return m_num_snp; };
    size_t get_num_sample() const{return m_num_sample;};
    std::vector<int> get_genotype(int geno) const;
    int get_distance() const{
    		return m_bp_list.back()-m_bp_list.front();
    };
    int get_first_bp() const{ return m_bp_list.front(); };
    int get_last_bp() const{return m_bp_list.back();};
    void clumping(std::map<std::string, bool> inclusion, std::vector<SNP> &snp_list, const std::map<std::string, size_t> &snp_index,
    					double p_threshold, double r2_threshold, size_t kb_threshold);

private:
    static std::mutex clump_mtx;
    double get_r2(const size_t i, const size_t j, bool adjust=false);
    bool openPlinkBinaryFile(const std::string s, std::ifstream & BIT);
    void clump_thread(const size_t index, const std::deque<size_t> &index_check, std::vector<SNP> &snp_list, const double r2_threshold);
    void compute_clump(const size_t index, size_t i_start, size_t i_end, std::vector<SNP> &snp_list, const std::deque<size_t> &index_check, const double r2_threshold);
    bool m_init;

#if defined(__LP64__)
    typedef uint64_t long_type;
#else
    typedef uint32_t long_type;
#endif
    std::string m_prefix;
    std::ifstream m_bed;
    std::ifstream m_bim;
    size_t m_num_sample;
    size_t m_num_snp;
    size_t m_num_bytes;
    size_t m_snp_iter;
    size_t m_bit_size;
    size_t m_required_bit;
    size_t m_thread;
    std::deque<std::string> m_chr_list;
    std::deque<std::string> m_snp_list;
    std::deque<size_t> m_cm_list;
    std::deque<size_t> m_bp_list;
    std::deque<std::string> m_ref_allele;
    std::deque<std::string> m_alt_allele;
    std::deque<double> m_maf;
    std::deque<long_type*> m_genotype;
    std::deque<long_type*> m_missing;
    std::deque<size_t> m_num_missing;
#if defined(__LP64__) || defined(_WIN64)
	#if defined(_WIN64)
		#define __LP64__
	#endif
    // LP64 machine, OS X or Linux
	#define ZEROLU 0LLU
	#define FIVEMASK ((~ZEROLU) / 3)

    const long_type THREE = 3LLU;
    const long_type THREEMASK = 0x3333333333333333LLU;
    const long_type OFMASK = 0x0f0f0f0f0f0f0f0fLLU;
    const long_type AAAAMASK = 0xaaaaaaaaaaaaaaaaLLU;
    const long_type ONEZEROMASK = 0x0101010101010101LLU;
	#define BITCT 64
	#define VEC_BYTES 16
	#define VEC_BITS (VEC_BYTES * 8)
    typedef union {
        __m128 vf;
		__m128i vi;
        __m128d vd;
      	uintptr_t u8[VEC_BITS / BITCT];
       	double d8[VEC_BYTES / sizeof(double)];
        	float f4[VEC_BYTES / sizeof(float)];
      	uint32_t u4[VEC_BYTES / sizeof(int32_t)];
      } __univec;

#else
    // 32-bit machine, Windows or Linux or OS X
    const long_type FIVEMASK = ((~0LU) / 3);
    const long_type THREE = 3LU;
    const long_type THREEMASK = 0x33333333;
    	const long_type OFMASK = 0x0f0f0f0f;
    	const long_type ONEZEROMASK = 0x01010101;
    	const long_type AAAAMASK = 0xaaaaaaaa;
#endif
    	//code from http://stackoverflow.com/a/17355341
    	static inline int popcnt128(__m128i n) {
    	    const __m128i n_hi = _mm_unpackhi_epi64(n, n);
    	    #ifdef _MSC_VER
    	        return __popcnt64(_mm_cvtsi128_si64(n)) + __popcnt64(_mm_cvtsi128_si64(n_hi));
    	    #else
    	        // I modified this into builtin_popcountll
    	        return __builtin_popcountll(_mm_cvtsi128_si64(n)) + __builtin_popcountll(_mm_cvtsi128_si64(n_hi));
    	    #endif
    	}
    	void ld_dot_prod_batch(__m128i* vec1, __m128i* vec2, __m128i* mask1, __m128i* mask2, int32_t* return_vals, uint32_t iters);
    	uint32_t ld_missing_ct_intersect(long_type* lptr1, long_type* lptr2, uintptr_t word12_ct, uintptr_t word12_rem, uintptr_t lshift_last);
    	static uint32_t popcount2_long(uintptr_t val) {
    	#ifdef __LP64__
    		val = (val & 0x3333333333333333LLU) + ((val >> 2) & 0x3333333333333333LLU);
    		return (((val + (val >> 4)) & 0x0f0f0f0f0f0f0f0fLLU) * 0x0101010101010101LLU) >> 56;
    	#else
    		val = (val & 0x33333333) + ((val >> 2) & 0x33333333);
    		return (((val + (val >> 4)) & 0x0f0f0f0f) * 0x01010101) >> 24;
    	#endif
    	}
};
#endif /* plink_hpp */