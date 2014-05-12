/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef _Tokenizer_H
#define _Tokenizer_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

#ifndef NCHAR
#	define NCHAR				255
#endif
#define CT_WHITESPACE		1
#define	CT_DIGIT			2
#define CT_ALPHA			4
#define CT_QUOTE			8
#define CT_COMMENT			16
#define TT_EOF				-1
#define	TT_EOL				'\n'
#define	TT_NUMBER			-2
#define TT_WORD				-3
#define TT_STRING			-4

/**
 * The tokenizer class takes an input stream and parses it
 * into "tokens", allowing the tokens to be read one at a time.
 * The parsing process is controlled by a table and a number
 * of flags that can be set to various states. The tokenizer
 * can recognize identifiers, numbers, quoted string and
 * various comment styles.
 */
class Tokenizer
{
private:
	std::istream&			m_input;							/**< Reference to input stream. */
	unsigned char			m_chType[NCHAR + 1];	/**< Table where each characters type is represented. */
	bool					m_eolIsSignificant;		/**< Flag if eol is significant, ie will return a token. */
	int						m_lineNo;							/**< Current line number. */
	bool					m_lowerCaseMode;			/**< If word tokens are to be forces lower case. */
	bool					m_pushedBack;					/**< If pushBack() has been called. */
	bool					m_slslComments;				/**< If slash-slash comments are enabled. */
	bool					m_slstComments;				/**< If slash-star comments are enabled. */
	int						m_ttype;							/**< Token type. */
	double					m_nval;								/**< Numeric value. */
	std::string				m_sval;								/**< String value. */

public:
	/**
	 * Initialize tokenizer by attaching an input
	 * stream. Also resets the tokenizer to default.
	 * @param input Reference to input stream.
	 */
	Tokenizer(std::istream& input);

	/**
	 * Virtual destructor so if this class is used as
	 * a base class, the destructors are called in
	 * proper order.
	 */
	virtual ~Tokenizer();

	/**
	 * Specifies which character starts a single-line comment.
	 * @param ch Single-line comment character.
	 */
	void commentChar(int ch);

	/**
	 * Determines whether or not ends of line are treated
	 * as tokens.
	 * @param flag True if eol are significant.
	 */
	void eolIsSignificant(bool flag);

	/**
	 * Return the current line number.
	 * @return Line number.
	 */
	int lineno(void);

	/**
	 * Determines whether or not word tokens are
	 * automatically lowercased.
	 * @param flag True if tokens shall be lowercased.
	 */
	void lowerCaseMode(bool flag);

	/**
	 * Parses the next token from the input stream
	 * of this tokenizer.
	 * @return The value of ttype member.
	 */
	int nextToken(void);

	/**
	 * Specifies that the character argument is "ordinary"
	 * in this tokenizer.
	 * @param ch Character.
	 */
	void ordinaryChar(int ch);

	/**
	 * Specifies that all characters c in the range
	 * low <= c <= hi are "ordinary" in this tokenizer.
	 * @param low Low range.
	 * @param hi High range.
	 */
	void ordinaryChars(int low, int hi);

	/**
	 * Specifies that numbers should be parsed
	 * by this tokenizer.
	 */
	void parseNumbers(void);

	/**
	 * Causes the next call to the nextToken method of this
	 * tokenizer to return the current value in the ttype member,
	 * and not to modify the value in nval or sval fields.
	 */
	void pushBack(void);

	/**
	 * Specifies that matching pairs of this character delimit
	 * string constants in this tokenizer.
	 * @param ch Quote character.
	 */
	void quoteChar(int ch);

	/**
	 * Reset the tokenizer's syntax table so that all characters
	 * are "ordinary". See the ordinaryChar method for more
	 * information on characters being ordinary.
	 */
	void resetSyntax(void);

	/**
	 * Determines whether or not the tokenizer recognizes
	 * C++-style comments.
	 */
	void slashSlashComments(bool flag);

	/**
	 * Determines whether or not the tokenizer recognizes
	 * C-style comments.
	 */
	void slashStarComments(bool flag);

	/**
	 * Specifies that all characters c in the range low <= c <= high,
	 * are white space characters.
	 */
	void whitespaceChars(int low, int hi);

	/**
	 * Specifies that all characters c in the range low <= c <= high,
	 * are word constituents.
	 */
	void wordChars(int low, int hi);

	/**
	 * Return token type.
	 * @return Value of m_ttype.
	 */
	inline int ttype(void) const {
		return m_ttype;
	}

	/**
	 * Return token number value.
	 * @return Value of m_nval;
	 */
	inline double	nval(void) const {
		return m_nval;
	}

	/**
	 * Return token string value.
	 * @return Value of m_sval;
	 */
	inline const std::string& sval(void) const {
		return m_sval;
	}
};

#endif	// _Tokenizer_H
