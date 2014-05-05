function iterate_directory()
{
	DIR=$2/*
	PROG=$1
	for f in $DIR
	do
		echo
		echo "==== --- |  Testing file '$f'  | --- ====="
		echo
		./$PROG $f
		echo
		echo '------- x -------'
		echo
	done
}
