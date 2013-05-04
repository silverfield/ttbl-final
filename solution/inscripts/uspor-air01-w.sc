#Preprocesses USP-OR on the 50 airport sub-timetable of the air01 dataset, then runs a comparison with TD Dijkstra (1000 queries)

load tt data/real/tt_air01-w.tt
mod tt rmover last
gen tt subtt last -n 50
gen tt 2td last
or td uspor last
or td dijkstra last
orcomp td last conn 1000 dijkstra uspor
quit
