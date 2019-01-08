PROJECT = shadertoy
LDLIBS = -lGL -lGLEW -lglut -lc $(if $(DEBUG),-lstdc++)
CXXFLAGS = -masm=intel -O1 -std=c++17 -no-pie -fno-plt -fno-stack-protector -nodefaultlibs $(if $(DEBUG),-DDEBUG -g)
# smol-v needs .got.plt and default libs

all: $(PROJECT)

.PHONY: run
run: all
	./$(PROJECT)

.PHONY: packer
packer: Makefile
	cd vondehi; nasm -fbin -o vondehi vondehi.asm

shader.min.frag: shader.frag replacements.pl Makefile
	glslangValidator -E shader.frag | perl replacements.pl > shader.min.frag
	glslangValidator shader.min.frag
	wc -c shader.min.frag

shader.min.frag.h: shader.min.frag Makefile
	printf '#pragma once\nconst char *shader_min_frag = R"--(' > shader.min.frag.h
	cat shader.min.frag >> shader.min.frag.h
	printf ')--";' >> shader.min.frag.h

shader.spv: shader.frag Makefile
	glslangValidator -G --auto-map-locations shader.frag -o shader.spv

smolv: smolv.cpp Makefile
	g++ -o smolv smolv.cpp smol-v/source/smolv.cpp

shader.smv: shader.spv smolv Makefile
	cat shader.spv | ./smolv e > shader.smv

shader.smv.h: shader.smv Makefile
	xxd -i shader.smv > shader.smv.h

main: main.cpp shader.min.frag.h Makefile
	g++ -o main main.cpp $(LDLIBS) $(CXXFLAGS)
ifndef DEBUG
	strip main
	strip -R .note -R .comment -R .eh_frame -R .eh_frame_hdr -R .note.gnu.build-id -R .got -R .got.plt -R .gnu.version -R .rela.dyn -R .shstrtab main
	# remove section header
	./Section-Header-Stripper/section-stripper.py main
	# clear out useless bits
	sed -i 's/_edata/\x00\x00\x00\x00\x00\x00/g' main;
	sed -i 's/__bss_start/\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00/g' main;
	sed -i 's/_end/\x00\x00\x00\x00/g' main;
endif
	wc -c main
	chmod +x main

main.gz: main Makefile
	-rm main.gz
	zopfli --gzip main
	wc -c main.gz

main.xz: main Makefile
	-rm main.xz
	lzma --format=lzma -9 --extreme --lzma1=preset=9,lc=0,lp=0,pb=0,nice=64,depth=4,dict=4096 --keep --stdout main > main.xz
	wc -c main.xz

$(PROJECT): main.xz packer Makefile
	cat ./vondehi/vondehi main.xz > $(PROJECT)
	chmod +x $(PROJECT)
	wc -c $(PROJECT)
