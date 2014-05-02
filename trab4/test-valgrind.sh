#!/bin/bash
function iterate_directory()
{
	DIR=$1/*
	for f in $DIR
	do
		echo
		echo "==== --- |  Testing $f  | --- ====="
		echo
		valgrind --leak-check=full --show-leak-kinds=all ./mini0-parser $f
		echo
		echo '------- x -------'
	done
}
iterate_directory testes-oficiais/
