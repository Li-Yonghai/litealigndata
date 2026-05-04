#include "baseCase/BaseCase.h"

using namespace testcase::CaseBaseCase;

TEST_P(CBaseCase, check_conf_ini_xml_fileMode_streamMode)
{
   std::cout << "=====TEST_P(CBaseCase, check_conf_ini_xml_fileMode_streamMode)======" << std::endl;
   ASSERT_TRUE(XmlToData());
   ASSERT_TRUE(Initialize());
   ASSERT_TRUE(CheckFileCount());
   ASSERT_TRUE(CheckFileLinesCount());
   ASSERT_TRUE(CheckFileItemValue());
   std::cout << "=====TEST_P(CBaseCase, check_conf_ini_xml_fileMode_streamMode)======" << std::endl;
}

INSTANTIATE_TEST_SUITE_P(litealigndata_Testcast, CBaseCase, ::testing::Values(
            "Example",
           "Ctrldata"
            ));

int main(int argc, char **argv)
{
   printf("\n****************************************************\n");
   printf("\n*                                                  *\n");
   printf("\n*        ");
   printf("\033[0m\033[1;33mwelcome using litealigndata case\033[0m");
   printf("          *\n");
   printf("\n*                                                  *\n");
   printf("\n****************************************************\n");
   
   testing::AddGlobalTestEnvironment(new CBaseCaseEnvironment);
   testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
