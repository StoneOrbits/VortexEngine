.PHONY: all clean rebuild preview

TEX=infographic.tex
PDF=infographic.pdf

all: $(PDF)

$(PDF): $(TEX)
	pdflatex -interaction=nonstopmode $(TEX)

clean:
	rm -f *.aux *.log *.out *.toc *.synctex.gz

rebuild: clean all

preview: all
	xdg-open $(PDF) || open $(PDF)

