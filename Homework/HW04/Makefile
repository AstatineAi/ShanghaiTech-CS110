# LaTeX Makefile

# Variables
LATEX = pdflatex
BIBTEX = bibtex
MAIN = main
OUTPUT_DIR = build

# Main target
all: $(OUTPUT_DIR) $(MAIN).pdf

# Create output directory
$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

# PDF generation
$(MAIN).pdf: $(MAIN).tex
	$(LATEX) -output-directory=$(OUTPUT_DIR) $(MAIN)
	if grep -q "\\\\bibliography{" $(MAIN).tex; then \
		$(BIBTEX) $(OUTPUT_DIR)/$(MAIN); \
		$(LATEX) -output-directory=$(OUTPUT_DIR) $(MAIN); \
		$(LATEX) -output-directory=$(OUTPUT_DIR) $(MAIN); \
	fi
	cp $(OUTPUT_DIR)/$(MAIN).pdf .

# Clean auxiliary files
clean:
	rm -f $(OUTPUT_DIR)/*.aux $(OUTPUT_DIR)/*.log $(OUTPUT_DIR)/*.toc
	rm -f $(OUTPUT_DIR)/*.lof $(OUTPUT_DIR)/*.lot $(OUTPUT_DIR)/*.bbl
	rm -f $(OUTPUT_DIR)/*.blg $(OUTPUT_DIR)/*.out $(OUTPUT_DIR)/*.nav
	rm -f $(OUTPUT_DIR)/*.snm $(OUTPUT_DIR)/*.vrb

# Clean all generated files
cleanall: clean
	rm -f $(MAIN).pdf $(OUTPUT_DIR)/$(MAIN).pdf

# Rebuild from scratch
rebuild: cleanall all

.PHONY: all clean cleanall rebuild