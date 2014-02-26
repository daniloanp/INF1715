#!/bin/bash
FILES=testes/*
for f in $FILES
do
	echo
	echo "============ --- |  Testing $f  | --- =====>>>>>>>>"
	echo ''
	./mini01-lex $f
	echo
	echo
	

	
	
done