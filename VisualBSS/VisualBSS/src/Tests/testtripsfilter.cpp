#include <bandit/bandit.h>
#include <tripsfilter.h>
#include <trip.h>

using namespace bandit;
using namespace snowhouse;

go_bandit([](){
    describe("test tripsfilter", [](){

        QVector<Trip> trips1(0);

        /* tn : create some trips for the tests*/
        Trip t1;
        t1.id = 1;
        t1.distance = 19;
        t1.duration = 400;
        t1.startDateTime = QDateTime(QDate(2016, 7, 7));
        t1.endDateTime =  QDateTime(QDate(2016, 7, 10));
        trips1.append(t1);

        Trip t2;
        t2.id = 2;
        t2.distance = 20;
        t2.duration = 400;
        t2.startDateTime = QDateTime(QDate(2016, 5, 5));
        t2.endDateTime =  QDateTime(QDate(2016, 5, 5));
        trips1.append(t2);

        it("Distance min", [&](){
            TripsFilterParams params;
            params.minDistance = 20;
            params.maxDistance = 100;
            params.minDuration = 0;
            params.maxDuration = 20000;
            params.fromPeriod = QDate(2016, 1, 1);
            params.toPeriod = QDate(2016, 12, 30);

            const TripsFilter tripfilter = TripsFilter(params);
            QVector<Trip> outTrips = tripfilter.filter(trips1);

            AssertThat(outTrips.size(), Equals(1));
            if(outTrips.size() == 1) AssertThat(outTrips[0].id, Equals(t2.id));
        });

        QVector<Trip> trips2(0);

        Trip t3;
        t3.id = 3;
        t3.distance = 180;
        t3.duration = 400;
        t3.startDateTime = QDateTime(QDate(2016, 7, 7));
        t3.endDateTime =  QDateTime(QDate(2016, 7, 10));
        trips2.append(t3);

        Trip t4;
        t4.id = 4;
        t4.distance = 181;
        t4.duration = 400;
        t4.startDateTime = QDateTime(QDate(2016, 5, 5));
        t4.endDateTime =  QDateTime(QDate(2016, 5, 5));
        trips2.append(t4);

        it("Distance max", [&](){
            TripsFilterParams params;
            params.minDistance = 20;
            params.maxDistance = 180;
            params.minDuration = 0;
            params.maxDuration = 20000;
            params.fromPeriod = QDate(2016, 1, 1);
            params.toPeriod = QDate(2016, 12, 30);

            const TripsFilter tripfilter = TripsFilter(params);
            QVector<Trip> outTrips = tripfilter.filter(trips2);

            AssertThat(outTrips.size(), Equals(1));
            if(outTrips.size() == 1) AssertThat(outTrips[0].id, Equals(t3.id));
        });

        it("invert distance min and max", [&](){
            TripsFilterParams params;
            params.minDistance = 180;
            params.maxDistance = 0;
            params.minDuration = 0;
            params.maxDuration = 20000;
            params.fromPeriod = QDate(2016, 1, 1);
            params.toPeriod = QDate(2016, 12, 30);

            const TripsFilter tripfilter = TripsFilter(params);
            QVector<Trip> outTrips = tripfilter.filter(trips1);

            AssertThat(outTrips.size(), Equals(0));
        });

        QVector<Trip> trips3(0);

        Trip t5;
        t5.id = 5;
        t5.distance = 100;
        t5.duration = 99;
        t5.startDateTime = QDateTime(QDate(2016, 5, 5));
        t5.endDateTime =  QDateTime(QDate(2016, 5, 5));
        trips3.append(t5);

        Trip t6;
        t6.id = 6;
        t6.distance = 100;
        t6.duration = 100;
        t6.startDateTime = QDateTime(QDate(2016, 5, 5));
        t6.endDateTime =  QDateTime(QDate(2016, 5, 5));
        trips3.append(t6);

        it("Duration min", [&](){
            TripsFilterParams params;
            params.minDistance = 0;
            params.maxDistance = 200;
            params.minDuration = 100;
            params.maxDuration = 500;
            params.fromPeriod = QDate(2016, 1, 1);
            params.toPeriod = QDate(2016, 12, 30);

            const TripsFilter tripfilter = TripsFilter(params);
            QVector<Trip> outTrips = tripfilter.filter(trips3);

            AssertThat(outTrips.size(), Equals(1));
            if(outTrips.size() == 1) AssertThat(outTrips[0].id, Equals(t6.id));
        });

        QVector<Trip> trips4(0);

        Trip t7;
        t7.id = 7;
        t7.distance = 100;
        t7.duration = 501;
        t7.startDateTime = QDateTime(QDate(2016, 5, 5));
        t7.endDateTime =  QDateTime(QDate(2016, 5, 5));
        trips4.append(t7);

        Trip t8;
        t8.id = 8;
        t8.distance = 100;
        t8.duration = 500;
        t8.startDateTime = QDateTime(QDate(2016, 5, 5));
        t8.endDateTime =  QDateTime(QDate(2016, 5, 5));
        trips4.append(t8);

        it("Duration max", [&](){
            TripsFilterParams params;
            params.minDistance = 0;
            params.maxDistance = 200;
            params.minDuration = 100;
            params.maxDuration = 500;
            params.fromPeriod = QDate(2016, 1, 1);
            params.toPeriod = QDate(2016, 12, 30);

            const TripsFilter tripfilter = TripsFilter(params);
            QVector<Trip> outTrips = tripfilter.filter(trips4);

            AssertThat(outTrips.size(), Equals(1));
            if(outTrips.size() == 1) AssertThat(outTrips[0].id, Equals(t8.id));
        });

        it("invert duration min and max", [&](){
            TripsFilterParams params;
            params.minDistance = 0;
            params.maxDistance = 200;
            params.minDuration = 100;
            params.maxDuration = 20;
            params.fromPeriod = QDate(2016, 1, 1);
            params.toPeriod = QDate(2016, 12, 30);

            const TripsFilter tripfilter = TripsFilter(params);
            QVector<Trip> outTrips = tripfilter.filter(trips4);

            AssertThat(outTrips.size(), Equals(0));
        });

        QVector<Trip> trips5(0);

        Trip t9;
        t9.id = 9;
        t9.distance = 100;
        t9.duration = 500;
        t9.startDateTime = QDateTime(QDate(2016, 5, 5));
        t9.endDateTime =  QDateTime(QDate(2016, 5, 7));
        trips5.append(t9);

        Trip t10;
        t10.id = 10;
        t10.distance = 100;
        t10.duration = 500;
        t10.startDateTime = QDateTime(QDate(2016, 5, 6));
        t10.endDateTime =  QDateTime(QDate(2016, 5, 7));
        trips5.append(t10);

        it("Date time min", [&](){
            TripsFilterParams params;
            params.minDistance = 0;
            params.maxDistance = 200;
            params.minDuration = 100;
            params.maxDuration = 600;
            params.fromPeriod = QDate(2016, 5, 6);
            params.toPeriod = QDate(2016, 7, 7);

            const TripsFilter tripfilter = TripsFilter(params);
            QVector<Trip> outTrips = tripfilter.filter(trips5);

            AssertThat(outTrips.size(), Equals(1));
            if(outTrips.size() == 1) AssertThat(outTrips[0].id, Equals(t10.id));
        });

        QVector<Trip> trips6(0);

        Trip t11;
        t11.id = 11;
        t11.distance = 100;
        t11.duration = 500;
        t11.startDateTime = QDateTime(QDate(2016, 7, 5));
        t11.endDateTime =  QDateTime(QDate(2016, 7, 7));
        trips6.append(t11);

        Trip t12;
        t12.id = 12;
        t12.distance = 100;
        t12.duration = 500;
        t12.startDateTime = QDateTime(QDate(2016, 5, 7));
        t12.endDateTime =  QDateTime(QDate(2016, 10, 7));
        trips6.append(t12);

        it("Date time max", [&](){
            TripsFilterParams params;
            params.minDistance = 0;
            params.maxDistance = 200;
            params.minDuration = 100;
            params.maxDuration = 600;
            params.fromPeriod = QDate(2016, 5, 6);
            params.toPeriod = QDate(2016, 7, 7);

            const TripsFilter tripfilter = TripsFilter(params);
            QVector<Trip> outTrips = tripfilter.filter(trips6);

            AssertThat(outTrips.size(), Equals(1));
            if(outTrips.size() == 1) AssertThat(outTrips[0].id, Equals(t11.id));
        });

        it("invert date min and max", [&](){
            TripsFilterParams params;
            params.minDistance = 0;
            params.maxDistance = 200;
            params.minDuration = 100;
            params.maxDuration = 600;
            params.fromPeriod = QDate(2016, 7, 7);
            params.toPeriod = QDate(2016, 5, 6);

            const TripsFilter tripfilter = TripsFilter(params);
            QVector<Trip> outTrips = tripfilter.filter(trips6);

            AssertThat(outTrips.size(), Equals(0));
        });
    });
});
