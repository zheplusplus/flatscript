#include <gtest/gtest.h>

#include <util/io.h>
#include <util/string.h>

TEST(IO, ReadFile)
{
    std::string s(util::read_file("LICENSE"));
    auto r = util::split_str(s, "\n");
    ASSERT_EQ(22, r.size());

    ASSERT_EQ("The MIT License (MIT)", r[0]);
    ASSERT_TRUE(r[1].empty());
    ASSERT_EQ("Copyright (c) 2012 Neuron Teckid", r[2]);

    ASSERT_EQ("OUT OF OR IN  CONNECTION WITH THE SOFTWARE OR "
              " THE USE OR OTHER DEALINGS IN THE", r[19]);
    ASSERT_EQ("SOFTWARE.", r[20]);
    ASSERT_TRUE(r[21].empty());
}

TEST(IO, ReadLargeFile)
{
    std::string s(util::read_file("codegen/gen-report.py"));
    auto r = util::split_str(s, "\n");
    int line = 641;
    ASSERT_EQ(line, r.size());

    ASSERT_EQ("import inspect", r[0]);
    ASSERT_TRUE(r[1].empty());

    ASSERT_EQ("write_errors_test_impl()", r[line - 2]);
    ASSERT_TRUE(r[line - 1].empty());
}
