latex: ../doku.tex ../lit.bib
	pdflatex ../doku.tex 
	makeglossaries doku
	biber doku
	pdflatex ../doku.tex
	pdflatex ../doku.tex

clean:
	rm doku*
