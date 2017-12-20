latex: ../bachelor.tex ../lit.bib ../implementierung.tex ../grundlagen.tex ../aufgabenstellung.tex ../glossary.tex
	pdflatex ../bachelor.tex 
	makeglossaries bachelor
	biber bachelor
	pdflatex ../bachelor.tex
	pdflatex ../bachelor.tex

clean:
	rm bachelor*
