load tt data/real/tt_air01-w.tt
mod tt rmover last
gen tt subtt last -n 50
gen tt 2td last
or td uspor last
or td dijkstra last
orcomp td last conn 1000 dijkstra uspor
quit
