#include <bandit/bandit.h>
#include <data.h>
using namespace bandit;
using namespace snowhouse;

go_bandit([](){
        describe("test Data", [](){

            const QString abs = "cheminabsversdossier";

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
                AssertThat(test2, Equals(false));
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
            const QString filename5 = abs + "invalidFile1.txt";
            bool test5 = data5.load(filename5, false);

            Data data6; // just somes columns intermix
            const QString filename6 = abs + "invalidFile2.txt";
            bool test6 = data6.load(filename5, false);

            Data data7;
            const QString filename7 = abs + "invalidFile3.txt";
            bool test7 = data7.load(filename5, false);

            it("Invalid file", [&](){
                AssertThat(test5, Equals(false));
                AssertThat(test6, Equals(false));
                AssertThat(test7, Equals(false));
            });




        });
});
