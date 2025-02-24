LAB_SUBDIRS = $(wildcard Lab/*/)
PROJECT_SUBDIRS = $(wildcard Project/*/)
SUBDIRS = $(LAB_SUBDIRS) $(PROJECT_SUBDIRS)

all:
	@echo "Run make in each subdirectory"

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

.PHONY: all clean