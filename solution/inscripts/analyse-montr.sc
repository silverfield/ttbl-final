#Analyses various properties (height, time range, overtaking) of the timetable montr and its graph forms (OC radius, highway dimension)

load tt data/real/tt_montr.tt
show tt last
an tt var last
an tt overtake last
gen tt 2te last
show te last
gen tt 2td last
an td conns last -apx 0.1
show td last
gen tt 2ug last
an ug hd last -prec 1
show ug last
quit
