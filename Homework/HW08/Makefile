# Makefile for LaTeX project

# The main document
MAIN = main

# Specify the LaTeX compiler
LATEX = xelatex

# Default target
all: $(MAIN).pdf

# Generate PDF
$(MAIN).pdf: $(MAIN).tex
	$(LATEX) $(MAIN)

# Clean auxiliary files
clean:
	rm -f *.aux *.log *.out *.toc *.lof *.lot *.bbl *.blg *.idx *.ilg *.ind *.brf *.fls *.fdb_latexmk *.synctex.gz

# Clean everything including the PDF
cleanall: clean
	rm -f $(MAIN).pdf

# Phony targets
.PHONY: all clean cleanall