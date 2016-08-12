#include <texpp/parser.h>

class TestBundle: public texpp::Bundle {
public:
    TestBundle(std::string mainFileName, boost::shared_ptr<std::istream> ifile)
        : m_mainFileName(mainFileName), m_ifile(ifile)
    {}

    std::string get_mainfile_name() {
        return m_mainFileName;
    }

    boost::shared_ptr<std::istream> get_file(const std::string& fname) {
        return m_ifile;
        if (fname == m_mainFileName) {

        }
    }

    std::string get_bib_filename(const std::string& fname) {
        return fname;
    }
    std::string get_tex_filename(const std::string& fname) {
        return fname;
    }
protected:
    std::string m_mainFileName;
    boost::shared_ptr<std::istream> m_ifile;
};
