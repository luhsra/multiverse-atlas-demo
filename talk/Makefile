export TEXINPUTS := ./texmf//:${TEXINPUTS}
TEX = latexmk -bibtex -outdir=build -pdflatex=lualatex
PDF = talk

all: ${PDF}.pdf


${PDF}.pdf: ${PDF}.tex secret-slides.tex
	$(TEX) -pdf $<
	@echo "make: copy link from build/ to $@"
	@cp -f build/$(notdir $@) $@

${PDF}.pdf.view: ${PDF}.tex 
	$(TEX) -pdf -pvc $< 

fig/%.pdf: fig/%.tex fig/common.tex
	@mkdir -p build
	$(TEX) -pdf $<
	@echo "make: copy link from build/ to $@"
	@cp -f build/$(notdir $@) $@


clean:
	$(TEX) -c
	$(TEX) -CA
	@rm -rf build ${PDF}.pdf

# nice on MacOS
view: all
	open $(PAPER).pdf
