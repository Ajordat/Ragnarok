LOGIN = ls30687
GRUP = x
CFLAGS = -Wall -Wextra -g
MAIN = main.c
MODULES = utils file_system_controller ext4
LIBS =
EXE = ragnarok

FILES = $(MAIN) $(patsubst %,%.c, $(MODULES))
HEADERS = $(LIBS) $(patsubst %,%.h, $(MODULES))


all: $(FILES) $(HEADERS)
	gcc $(FILES) $(CFLAGS) -o $(EXE)

stop:
	@ps -u $(LOGIN) | grep -e $(EXE) -e memcheck-amd64- | awk '{print $$1}' | xargs kill -9 2> /dev/null
	@echo "Service stopped."

again: clean all

clean:
	rm -f $(EXE)

tar:
	tar -cf G$(GRUP)_ragnarok_$(LOGIN).tar $(FILES) $(HEADERS) makefile
