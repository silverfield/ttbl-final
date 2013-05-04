#Preprocesses USP-OR-A + locsep on the 1000 station sub-timetable of the sncf dataset, then runs a comparison with TD Dijkstra (1000 queries)

load tt data/real/tt_sncf.tt
mod tt rmover last
gen tt subtt last -n 1000
gen tt 2td last
gen td 2ug last
pb ug hdeg last hdeg -l 10
pb ug locsep last locsep -set hdeg
or td dijkstra last
or td usporaseg last -set locsep
orcomp td last conn 1000 dijkstra uspora
quit
