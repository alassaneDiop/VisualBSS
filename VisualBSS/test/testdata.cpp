#include <bandit/bandit.h>
#include <data.h>
using namespace bandit;
using namespace snowhouse;

go_bandit([](){
        describe("test Data", [](){

            Data data1;
            const QString filename1 = "cheminabs/oneTrip.csv";
            bool test1 = data1.load(filename1, false);

            Data data2;
            const QString filename2 = "cheminabs/withoutAnyTrips.csv";
            bool test2 = data2.load(filename2, false);

            Data data3;
            const QString filename3 = "cheminabs/someTrips.csv";
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
        });
});
