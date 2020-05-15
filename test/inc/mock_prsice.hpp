#ifndef MOCK_PRSICE_HPP
#define MOCK_PRSICE_HPP
#include "catch.hpp"
#include "prsice.hpp"
class mock_prsice : public PRSice
{
public:
    mock_prsice(Reporter* reporter) { set_reporter(reporter); }
    mock_prsice(const bool binary, Reporter* reporter)
        : PRSice(CalculatePRS(), PThresholding(), Permutations(), "PRSice", 3,
                 3, binary, reporter)
    {
    }
    mock_prsice() {};
    mock_prsice(const CalculatePRS& prs_info, const PThresholding& p_info,
                const Permutations& perm, const std::string& output,
                const size_t max_fid, const size_t max_iid, const bool binary,
                Reporter* reporter)
        : PRSice(prs_info, p_info, perm, output, max_fid, max_iid, binary,
                 reporter)
    {
    }
    void set_reporter(Reporter* reporter) { m_reporter = reporter; }

    static std::tuple<size_t, bool>
    test_get_pheno_idx(const std::vector<std::string_view>& column,
                       const Phenotype& pheno_info, const std::string& pheno)
    {
        return PRSice::get_pheno_idx(column, pheno_info, pheno);
    }
    static void
    test_parse_pheno_header(std::unique_ptr<std::istream> pheno_file,
                            Phenotype& pheno_info, Reporter& reporter)
    {
        parse_pheno_header(std::move(pheno_file), pheno_info, reporter);
    }
    Phenotype get_pheno_info() const { return m_pheno_info; }
    std::tuple<size_t, size_t> get_progress()
    {
        return {m_total_process, m_total_competitive_process};
    }
    std::tuple<size_t, size_t> get_current_progress()
    {
        return {m_analysis_done, m_total_competitive_perm_done};
    }
    void set_progress(size_t analysis_done, size_t competitive_done)
    {
        m_analysis_done = analysis_done;
        m_total_competitive_perm_done = competitive_done;
    }
    std::unordered_map<std::string, std::string>
    test_load_pheno_map(const std::string& delim, const size_t idx,
                        const bool ignore_fid,
                        std::unique_ptr<std::istream> pheno_file)
    {
        return load_pheno_map(delim, idx, ignore_fid, std::move(pheno_file));
    }
    void test_parse_pheno(const std::string& pheno,
                          std::vector<double>& pheno_store, int& max_pheno_code)
    {
        parse_pheno(pheno, pheno_store, max_pheno_code);
    }
    bool
    test_quantitative_pheno_is_valid(const std::vector<double>& pheno_store)
    {
        return quantitative_pheno_is_valid(pheno_store);
    }

    std::tuple<bool, size_t, size_t>
    test_binary_pheno_is_valid(const int max_pheno_code,
                               std::vector<double>& pheno_store)
    {
        return binary_pheno_is_valid(max_pheno_code, pheno_store);
    }
};

#endif // MOCK_PRSICE_HPP
