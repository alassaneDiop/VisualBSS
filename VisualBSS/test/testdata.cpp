#include <bandit/bandit.h>
#include <data.h>
using namespace bandit;
using namespace snowhouse;

go_bandit([](){
        describe("test Data", [](){

            const QString abs = "cheminabs";

            Data data1;
            const QString filename1 = abs + "oneTrip.csv";
            bool test1 = data1.load(filename1, false);

            Data data2;
            const QString filename2 = abs + "withoutAnyTrips.csv";
            bool test2 = data2.load(filename2, false);

            Data data3;
            const QString filename3 = abs + "someTrips.csv";
            bool test3 = data3.load(filename3, false);


            it("No trip", [&](){
                AssertThat(test2, Equals(true));
                AssertThat(data2.hasData(), Equals(false));
            });

            it("One trip", [&](){
                AssertThat(test1, Equals(true));
                AssertThat(data1.hasData(), Equals(true));
            });

            it("Some trips", [&](){
                AssertThat(test3, Equals(true));
                AssertThat(data3.hasData(), Equals(true));
            });

            Data data4;
            const QString filename4 = abs + "mauvaiseExtension.txt";
            bool test4 = data4.load(filename4, false);

            it("Bad extension", [&](){
                AssertThat(test4, Equals(false));
            });

            Data data5; // completely random
            const QString filename5 = abs + "invalidFile1.csv";
            bool test5 = data5.load(filename5, false);

            Data data6; // just somes columns intermix
            const QString filename6 = abs + "invalidFile2.csv";
            bool test6 = data6.load(filename5, false);

            Data data7;
            const QString filename7 = abs + "invalidFile3.csv";
            bool test7 = data7.load(filename5, false);

            it("Invalid file", [&](){
                AssertThat(test5, Equals(true));
                AssertThat(data5.hasData(), Equals(false));
                AssertThat(test6, Equals(true));
                AssertThat(data6.hasData(), Equals(false));
                AssertThat(test7, Equals(true));
                AssertThat(data7.hasData(), Equals(false));
            });

            Data data8;
            const QString filename8 = abs + "fileNotfound.csv";
            bool test8 = data8.load(filename5, false);

            it("file not found", [&](){
                AssertThat(test8, Equals(false));
            });




        });
});
