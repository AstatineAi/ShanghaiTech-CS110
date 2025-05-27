TEXFILE = homework7
OUTPUT = $(TEXFILE).pdf
SRCDIRS = src/
SRCTEX = $(wildcard $(SRCDIRS)*.tex)
DEPS = $(TEXFILE).tex $(SRCTEX)
LATEX = xelatex
BIBTEX = bibtex
BUILDDIR = build

# Main target
all: $(OUTPUT)

# Build the PDF
$(OUTPUT): $(DEPS)
	mkdir -p $(BUILDDIR)
	$(LATEX) -output-directory=$(BUILDDIR) $(TEXFILE).tex
	@if grep -q "\\\\bibliography{" $(TEXFILE).tex; then \
		cd $(BUILDDIR) && $(BIBTEX) $(TEXFILE); \
		cd .. && $(LATEX) -output-directory=$(BUILDDIR) $(TEXFILE).tex; \
	fi
	$(LATEX) -output-directory=$(BUILDDIR) $(TEXFILE).tex
	cp $(BUILDDIR)/$(OUTPUT) .

# Clean build files
clean:
	rm -rf $(BUILDDIR)
	rm -f $(OUTPUT)

# Clean and rebuild
rebuild: clean all

.PHONY: all clean rebuild