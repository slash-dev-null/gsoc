NAME := gpu_player
SRC := gpu_player.c
PATHS := -I"../libavcodec" \
	-I"../libavformat" \
	-I"../libavutil" \
	-I".." \
	-L"../libavdevice" \
	-L"../libavformat" \
	-L"../libavcodec" \
	-L"../libavutil"  
CFLAGS := -g3 -O0
LIBS :=  -lz -lm -ldl -ldl -L/usr/lib -lSDL -lGL -lGLEW -lglut \
	-lavformat -lavcodec -lavdevice -lavutil  -lbz2
#FILE :=../saikano-ova-h264-blocking-01.mkv  
#FILE := ../../Ganges.E01.1080p.L4.1.mkv
FILE := ../../hellsing-h264-blocking.mkv

default: compile run

compile:
	cd ..; make; cd gpu;
	gcc $(SRC) $(CFLAGS) $(PATHS) $(LIBS) -o $(NAME)

run:
	vglrun ./$(NAME) $(FILE) 5 motion
