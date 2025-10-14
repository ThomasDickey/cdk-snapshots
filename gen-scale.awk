# $Id: gen-scale.awk,v 1.1 2025/10/14 23:09:39 tom Exp $
# ---------------------------------------------------------------------------
# Copyright 2025 Thomas E. Dickey
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, distribute with
# modifications, sublicense, and/or sell copies of the Software, and to permit
# persons to whom the Software is furnished to do so, subject to the following
# conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
# ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
# IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# Except as contained in this notice, the name(s) of the above copyright
# holders shall not be used in advertising or otherwise to promote the sale,
# use or other dealings in this Software without prior written authorization.
# ---------------------------------------------------------------------------
# Generate variations from gen-scale.c and gen-slider.c for various datatypes.

BEGIN {
	UPPER = toupper(MIXED);
	CTYPE = tolower(DTYPE);

	# Some widgets provide for setting the digits.  Turn on/off the
	# corresponding ifdef.
	CHECK = "<INT>";
	if (CTYPE == "float" || match(CTYPE,"^.*double$") > 0) {
		CHECK = "<FLOAT>";
		PRINT = "f";
	} else if (match(CTYPE, "^long[ \t]+long$") > 0) {
		PRINT = "ll";
	} else if (match(CTYPE, "^unsigned[ \t]+long$") > 0) {
		PRINT = "lu";
	} else if (CTYPE == "unsigned") {
		PRINT = "u";
	} else if (CTYPE == "long") {
		PRINT = "l";
		;;
	} else {
		PRINT = "d";
	}

	if ( PRINT == "d" ) {
		MODEL = ""; 
	} else if ( CTYPE == "double" ) {
		MODEL = "d";
	} else {
		MODEL = PRINT;
	}
	IFDEF = 1;
}

/^.XX #(if|endif)/ {
	sub("^.XX ","");
}

/^#if/ {
	if ( CHECK == $2 ) {
		IFDEF = 1;
	} else {
		IFDEF = 0;
	}
	next;
}

/^#endif/ {
	IFDEF = 1;
	next;
}

{
	if ( IFDEF == 0 ) {
		next;
	}
	gsub("<UPPER>", UPPER);
	gsub("<MIXED>", MIXED);
	gsub("<DTYPE>", DTYPE);
	gsub("<CTYPE>", CTYPE);
	gsub("<MODEL>", MODEL);
	gsub("<PRINT>", PRINT);
	print;
}

# vile:ts=4 sw=4
