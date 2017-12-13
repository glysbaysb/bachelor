latex: ../doku.tex ../lit.bib ../implementierung.tex ../grundlagen.tex ../aufgabenstellung.tex ../glossary.tex
	pdflatex ../doku.tex 
	makeglossaries doku
	biber doku
	pdflatex ../doku.tex
	pdflatex ../doku.tex

clean:
	rm doku*
