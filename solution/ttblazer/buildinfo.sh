#!/bin/bash

src=src
bin=bin
name=ttblazer
curdir=`pwd`
commonflags='-g -I '$curdir'/../boostlib'
mainflags="-lpthread "$commonflags
#			pathnamesource-file(s)...
i=0
objects[$i]='dummy_template dummy_template'
flags[$i]=$commonflags
i=$i+1
objects[$i]='../../common/src/logger logger'
flags[$i]=$commonflags
i=$i+1
objects[$i]='../../common/src/algtimer algtimer'
flags[$i]=$commonflags
i=$i+1
objects[$i]='../../common/src/structsizer structsizer'
flags[$i]=$commonflags
i=$i+1
objects[$i]='../../common/src/sender sender'
flags[$i]=$commonflags
i=$i+1
objects[$i]='../../common/src/receiver receiver'
flags[$i]=$commonflags
i=$i+1
objects[$i]='../../common/src/common common'
flags[$i]=$commonflags
i=$i+1
objects[$i]='cmdproc cmdproc'
flags[$i]=$commonflags
i=$i+1
objects[$i]='communicator communicator'
flags[$i]=$commonflags
i=$i+1
objects[$i]='main main'
flags[$i]=$commonflags
i=$i+1
objects[$i]='algdijkstra algdijkstra'
flags[$i]=$commonflags
i=$i+1
objects[$i]='algorithms algorithms'						#ONLY PART IS TEMPLATE
flags[$i]=$commonflags
i=$i+1
objects[$i]='algaccn algaccn'
flags[$i]=$commonflags
i=$i+1
objects[$i]='algusp algusp'
flags[$i]=$commonflags
i=$i+1
objects[$i]='neuralnet neuralnet'
flags[$i]=$commonflags
i=$i+1
objects[$i]='structures structures'
flags[$i]=$commonflags
i=$i+1
objects[$i]='central central'
flags[$i]=$commonflags
i=$i+1
objects[$i]='options options'
flags[$i]=$commonflags
i=$i+1
objects[$i]='tests tests'
flags[$i]=$commonflags
i=$i+1
objects[$i]='cmdlnproc cmdlnproc'
flags[$i]=$commonflags
i=$i+1
objects[$i]='manipulator manipulator'
flags[$i]=$commonflags
i=$i+1
objects[$i]='fibheap fibheap'
flags[$i]=$commonflags
i=$i+1
#objects[$i]='objects/graph graph'						IS TEMPLATE
#flags[$i]=$commonflags
#i=$i+1
objects[$i]='objects/undergr undergr'
flags[$i]=$commonflags
i=$i+1
objects[$i]='objects/timetable timetable'
flags[$i]=$commonflags
i=$i+1
objects[$i]='objects/timeexp timeexp'
flags[$i]=$commonflags
i=$i+1
objects[$i]='objects/timedep timedep'
flags[$i]=$commonflags
i=$i+1
objects[$i]='objects/object object'
flags[$i]=$commonflags
i=$i+1
objects[$i]='oracles/ug_dijkstra ug_dijkstra'
flags[$i]=$commonflags
i=$i+1
objects[$i]='oracles/tg_neural tg_neural'
flags[$i]=$commonflags
i=$i+1
objects[$i]='oracles/tg_neuralea tg_neuralea'
flags[$i]=$commonflags
i=$i+1
objects[$i]='oracles/tg_neuralrout tg_neuralrout'
flags[$i]=$commonflags
i=$i+1
objects[$i]='oracles/td_uspor td_uspor'
flags[$i]=$commonflags
i=$i+1
objects[$i]='oracles/td_usporseg td_usporseg'
flags[$i]=$commonflags
i=$i+1
objects[$i]='oracles/td_uspora td_uspora'
flags[$i]=$commonflags
i=$i+1
objects[$i]='oracles/td_usporaseg td_usporaseg'
flags[$i]=$commonflags
i=$i+1
objects[$i]='oracles/td_uspora3 td_uspora3'
flags[$i]=$commonflags
i=$i+1
objects[$i]='oracles/tg_dijkstra tg_dijkstra'
flags[$i]=$commonflags
i=$i+1
objects[$i]='oracles/td_dijkstra td_dijkstra'
flags[$i]=$commonflags
i=$i+1
objects[$i]='oracles/oracleinfos oracleinfos'
flags[$i]=$commonflags
i=$i+1
objects[$i]='analysers/tryout tryout'
flags[$i]=$commonflags
i=$i+1
objects[$i]='analysers/ug_connectivity ug_connectivity'
flags[$i]=$commonflags
i=$i+1
objects[$i]='analysers/ug_strconn ug_strconn'
flags[$i]=$commonflags
i=$i+1
objects[$i]='analysers/ug_degrees ug_degrees'
flags[$i]=$commonflags
i=$i+1
objects[$i]='analysers/ug_paths ug_paths'
flags[$i]=$commonflags
i=$i+1
objects[$i]='analysers/ug_accnodes ug_accnodes'
flags[$i]=$commonflags
i=$i+1
objects[$i]='analysers/tg_conns tg_conns'
flags[$i]=$commonflags
i=$i+1
objects[$i]='analysers/ug_betwess ug_betwess'
flags[$i]=$commonflags
i=$i+1
objects[$i]='analysers/gr_hdim gr_hdim'
flags[$i]=$commonflags
i=$i+1
objects[$i]='analysers/te_betwess te_betwess'
flags[$i]=$commonflags
i=$i+1
objects[$i]='analysers/tt_overtake tt_overtake'
flags[$i]=$commonflags
i=$i+1
objects[$i]='analysers/te_connectivity te_connectivity'
flags[$i]=$commonflags
i=$i+1
objects[$i]='analysers/tg_usp tg_usp'
flags[$i]=$commonflags
i=$i+1
objects[$i]='analysers/to_various to_various'
flags[$i]=$commonflags
i=$i+1
objects[$i]='analysers/ug_density ug_density'
flags[$i]=$commonflags
i=$i+1
objects[$i]='analysers/analyserinfos analyserinfos'
flags[$i]=$commonflags
i=$i+1
objects[$i]='generators/generatorinfos generatorinfos'
flags[$i]=$commonflags
i=$i+1
objects[$i]='generators/tt_toug tt_toug'
flags[$i]=$commonflags
i=$i+1
objects[$i]='generators/tt_totd tt_totd'
flags[$i]=$commonflags
i=$i+1
objects[$i]='generators/tt_tote tt_tote'
flags[$i]=$commonflags
i=$i+1
objects[$i]='generators/tt_subtt tt_subtt'
flags[$i]=$commonflags
i=$i+1
objects[$i]='generators/te_toug te_toug'
flags[$i]=$commonflags
i=$i+1
objects[$i]='generators/td_toug td_toug'
flags[$i]=$commonflags
i=$i+1
objects[$i]='generators/ug_subcon ug_subcon'
flags[$i]=$commonflags
i=$i+1
objects[$i]='generators/ug_strcomp ug_strcomp'
flags[$i]=$commonflags
i=$i+1
objects[$i]='modifiers/modifierinfos modifierinfos'
flags[$i]=$commonflags
i=$i+1
objects[$i]='modifiers/tt_rmovertake tt_rmovertake'
flags[$i]=$commonflags
i=$i+1
objects[$i]='postbox/postboxinfos postboxinfos'
flags[$i]=$commonflags
i=$i+1
objects[$i]='postbox/postbox postbox'
flags[$i]=$commonflags
i=$i+1
objects[$i]='postbox/ug_highbc ug_highbc'
flags[$i]=$commonflags
i=$i+1
objects[$i]='postbox/ug_anhdeg ug_anhdeg'
flags[$i]=$commonflags
i=$i+1
objects[$i]='postbox/ug_anhbc ug_anhbc'
flags[$i]=$commonflags
i=$i+1
objects[$i]='postbox/ug_highdeg ug_highdeg'
flags[$i]=$commonflags
i=$i+1
objects[$i]='postbox/tg_junctions tg_junctions'
flags[$i]=$commonflags
i=$i+1
objects[$i]='postbox/tg_usp_pb tg_usp_pb'
flags[$i]=$commonflags
i=$i+1
objects[$i]='postbox/ug_locsep ug_locsep'
flags[$i]=$commonflags
