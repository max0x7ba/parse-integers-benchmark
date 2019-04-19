SHELL := /bin/bash
BUILD := release

TOOLSET := gcc
build_dir := ${CURDIR}/${BUILD}/${TOOLSET}

CXX.gcc := g++
CC.gcc := gcc
LD.gcc := g++
AR.gcc := gcc-ar

CXX.gcc-8 := g++-8
CC.gcc-8 := gcc-8
LD.gcc-8 := g++-8
AR.gcc-8 := gcc-ar-8

CXX.clang := clang++
CC.clang := clang
LD.clang := clang++
AR.clang := ar

CXX := ${CXX.${TOOLSET}}
CC := ${CC.${TOOLSET}}
LD := ${LD.${TOOLSET}}
AR := ${AR.${TOOLSET}}

CXXFLAGS.gcc.debug := -Og -fstack-protector-all -fno-omit-frame-pointer # -D_GLIBCXX_DEBUG
CXXFLAGS.gcc.release := -O3 -march=native -ffast-math -DNDEBUG
CXXFLAGS.gcc := -pthread -std=gnu++1z -march=native -W{all,extra,error} -g -fmessage-length=0 ${CXXFLAGS.gcc.${BUILD}}
CXXFLAGS.gcc-8 := ${CXXFLAGS.gcc}

CFLAGS.gcc := -pthread -march=native -W{all,extra} -g -fmessage-length=0 ${CXXFLAGS.gcc.${BUILD}}
CFLAGS.gcc-8 := ${CFLAGS.gcc}

CXXFLAGS.clang.debug := -O0 -fstack-protector-all
CXXFLAGS.clang.release := -O3 -march=native -ffast-math -DNDEBUG
CXXFLAGS.clang := -pthread -std=gnu++1z -march=native -W{all,extra,error} -g -fmessage-length=0 ${CXXFLAGS.clang.${BUILD}}

CXXFLAGS := ${CXXFLAGS.${TOOLSET}}
CFLAGS := ${CFLAGS.${TOOLSET}}

CPPFLAGS :=

#LDFLAGS.gcc := -L${gcc_dir}/lib64 -Wl,-rpath=${gcc_dir}/lib64
LDFLAGS.debug :=
LDFLAGS.release :=
LDFLAGS := -fuse-ld=gold -pthread -g ${LDFLAGS.${BUILD}} ${LDFLAGS.${TOOLSET}}
LDLIBS := -lrt

COMPILE.CXX = ${CXX} -c -o $@ ${CPPFLAGS} -MD -MP ${CXXFLAGS} $(abspath $<)
COMPILE.S = ${CXX} -S -masm=intel -o- ${CPPFLAGS} ${CXXFLAGS} $(abspath $<) | c++filt > $@
PREPROCESS.CXX = ${CXX} -E -o $@ ${CPPFLAGS} ${CXXFLAGS} $(abspath $<)
COMPILE.C = ${CC} -c -o $@ ${CPPFLAGS} -MD -MP ${CFLAGS} $(abspath $<)
LINK.EXE = ${LD} -o $@ $(LDFLAGS) $(filter-out Makefile,$^) $(LDLIBS)
LINK.SO = ${LD} -shared -o $@ $(LDFLAGS) $(filter-out Makefile,$^) $(LDLIBS)
LINK.A = ${AR} rsc $@ $(filter-out Makefile,$^)

all : ${build_dir}/parse_integers_fast

${build_dir} :
	mkdir -p $@

${build_dir}/parse_integers_fast : ${build_dir}/parse_integers_fast.o Makefile | ${build_dir}
	$(strip ${LINK.EXE})
-include ${build_dir}/parse_integers_fast.d

${build_dir}/%.so : CXXFLAGS += -fPIC
${build_dir}/%.so : Makefile | ${build_dir}
	$(strip ${LINK.SO})

${build_dir}/%.a : Makefile | ${build_dir}
	$(strip ${LINK.A})

random_ints_1600000.txt :
	echo 1600000 0 >$@
	od --address-radix=n --read-bytes=$$((1600000*4)) --format=d4 /dev/urandom >>$@

run_parse_integers_fast : ${build_dir}/parse_integers_fast | random_ints_1600000.txt
	@echo "---- running $< ----"
	$<

${build_dir}/%.o : %.cc Makefile | ${build_dir}
	$(strip ${COMPILE.CXX})

${build_dir}/%.o : %.c Makefile | ${build_dir}
	$(strip ${COMPILE.C})

%.S : %.cc Makefile | ${build_dir}
	$(strip ${COMPILE.S})

%.I : %.cc
	$(strip ${PREPROCESS.CXX})

clean :
	rm -rf ${build_dir}

.PHONY : clean all run_parse_integers_fast run_%
