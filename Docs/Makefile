all:
	pdflatex cahier_des_besoins.tex
	bibtex cahier_des_besoins.aux
	pdflatex cahier_des_besoins.tex
	pdflatex cahier_des_besoins.tex
	evince cahier_des_besoins.pdf &
	

clean:
	rm -f *.log *.aux *.dvi *.bbl *.blg *.out *.toc
