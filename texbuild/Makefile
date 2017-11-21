latex: ../doku.tex ../lit.bib
	pdflatex ../doku.tex 
	biber doku
	#makeglossaries doku
	pdflatex ../doku.tex
	pdflatex ../doku.tex

clean:
	rm doku*
