bachelor: ../bachelor.tex ../lit.bib ../implementierung.tex ../grundlagen.tex ../aufgabenstellung.tex ../glossary.tex 
	pdflatex ../bachelor.tex 
	makeglossaries bachelor
	biber bachelor
	pdflatex ../bachelor.tex
	pdflatex ../bachelor.tex
bedienungsanleitung: ../bedienungsanleitungs.tex
	pdflatex ../bedienungsanleitungs.tex
	pdflatex ../bedienungsanleitungs.tex
neu: ../sections.tex
	pdflatex ../sections.tex
	pdflatex ../sections.tex
clean:
	rm bedienungsanleitungs*
	rm bachelor*
