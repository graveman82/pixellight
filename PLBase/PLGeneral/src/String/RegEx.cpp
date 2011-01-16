/*********************************************************\
 *  File: RegEx.cpp                                      *
 *
 *  Copyright (C) 2002-2011 The PixelLight Team (http://www.pixellight.org/)
 *
 *  This file is part of PixelLight.
 *
 *  PixelLight is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PixelLight is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with PixelLight. If not, see <http://www.gnu.org/licenses/>.
\*********************************************************/


// [TODO] This is just a quick and simple PCRE wrapper. Could need some more attention, e.g.:
//		  - Check the maximum number of values and create a big enough output vector
//		  - Check for unicode strings and use the proper UTF8 functions
//		  - Replace function


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <pcre.h>
#include "PLGeneral/String/RegEx.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLGeneral {


//[-------------------------------------------------------]
//[ Definitions                                           ]
//[-------------------------------------------------------]
// Maximum number of arguments we can set
static const int MaxGroups	= 11;
static const int VecSize	= (MaxGroups+1) * 3;


//[-------------------------------------------------------]
//[ Public static functions                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Converts a given wildcard into a regular expression
*/
String RegEx::WildcardToRegEx(const String &sWildcard)
{
	// Begin
	String sString = '^';

	// Convert
	if  (sWildcard.GetFormat() == String::ASCII) {
		// Loop through all characters
		for (uint32 i=0, nLength=sWildcard.GetLength(); i<nLength; i++) {
			const char szCharacter = sWildcard.GetASCII()[i];
			switch (szCharacter) {
				case '*':
					sString += ".*";
					break;

				case '?':
					sString += '.';
					break;

				// Escape special regex-characters
				case '(': case ')': case '[': case ']': case '$':
				case '^': case '.': case '{': case '}': case '|':
				case '\\':
					sString += '\\';
					sString += szCharacter;
					break;

				default:
					sString += szCharacter;
					break;
			}
		}
	} else {
		// Loop through all characters
		for (uint32 i=0, nLength=sWildcard.GetLength(); i<nLength; i++) {
			const wchar_t szCharacter = sWildcard.GetUnicode()[i];
			switch (szCharacter) {
				case L'*':
					sString += L".*";
					break;

				case L'?':
					sString += L'.';
					break;

				// Escape special regex-characters
				case L'(': case L')': case L'[': case L']': case L'$':
				case L'^': case L'.': case L'{': case L'}': case L'|':
				case L'\\':
					sString += L'\\';
					sString += szCharacter;
					break;

				default:
					sString += szCharacter;
					break;
			}
		}
	}

	// End
	sString += '$';

	// Return the regular expression
	return sString;
}


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
RegEx::RegEx() :
	m_pPCRE(nullptr),
	m_pExtra(nullptr),
	m_nFlags(MatchCaseSensitive | MatchGreedy),
	m_nPosition(0)
{
}

/**
*  @brief
*    Constructor
*/
RegEx::RegEx(const String &sExpression, uint32 nFlags) :
	m_pPCRE(nullptr),
	m_pExtra(nullptr),
	m_nFlags(nFlags),
	m_nPosition(0)
{
	// Set expression
	SetExpression(sExpression, nFlags);
}

/**
*  @brief
*    Copy constructor
*/
RegEx::RegEx(const RegEx &cRegEx) :
	m_pPCRE(nullptr),
	m_pExtra(nullptr),
	m_nFlags(cRegEx.m_nFlags),
	m_nPosition(0)
{
	// Copy expression
	SetExpression(cRegEx.m_sExpression, cRegEx.m_nFlags);
}

/**
*  @brief
*    Destructor
*/
RegEx::~RegEx()
{
	// Release expression
	FreePCRE();
}

/**
*  @brief
*    Assignment operator
*/
RegEx &RegEx::operator =(const RegEx &cRegEx)
{
	// Copy expression
	SetExpression(cRegEx.m_sExpression, cRegEx.m_nFlags);

	// Return this instance
	return *this;
}

/**
*  @brief
*    Get regular expression
*/
String RegEx::GetExpression() const
{
	// Return expression
	return m_sExpression;
}

/**
*  @brief
*    Set regular expression
*/
void RegEx::SetExpression(const String &sExpression, uint32 nFlags)
{
	// Save expression and options
	m_sExpression	= sExpression;
	m_nFlags		= nFlags;

	// Any encoding flags set by the user?
	const bool bEncodingASCII = (m_nFlags & EncodingASCII) != 0;
	const bool bEncodingUTF8  = (m_nFlags & EncodingUTF8)  != 0;

	// ASCII and UTF8 flags set at the same time?
	if (bEncodingASCII && bEncodingUTF8)
		m_nFlags |= ~EncodingASCII; // Remove the ASCII flag!

	// Get the encoding automatically?
	else if (!bEncodingASCII && !bEncodingUTF8)
		m_nFlags |= (sExpression.GetFormat() != String::ASCII) ? EncodingUTF8 : EncodingASCII;

	// Compile expression
	CompilePCRE();
}

/**
*  @brief
*    Check if the regular expression is valid
*/
bool RegEx::IsValid() const
{
	// Return valid flag
	return (m_pPCRE != nullptr);
}

/**
*  @brief
*    Get mode and encoding flags
*/
uint32 RegEx::GetFlags() const
{
	return m_nFlags;
}

/**
*  @brief
*    Is the expression case sensitive?
*/
bool RegEx::IsCaseSensitive() const
{
	return ((m_nFlags & MatchCaseSensitive) != 0);
}

/**
*  @brief
*    Is the expression greedy?
*/
bool RegEx::IsGreedy() const
{
	return ((m_nFlags & MatchGreedy) != 0);
}

/**
*  @brief
*    Is ASCII encoding used?
*/
bool RegEx::IsASCII() const
{
	return ((m_nFlags & EncodingASCII) != 0);
}

/**
*  @brief
*    Is UTF8 encoding used?
*/
bool RegEx::IsUTF8() const
{
	return ((m_nFlags & EncodingUTF8) != 0);
}

/**
*  @brief
*    Analyse the regular expression and store additional internal information
*/
void RegEx::Study()
{
	if (m_pPCRE) {
		const char *pszError;
		m_pExtra = pcre_study(m_pPCRE, 0, &pszError);
	}
}

/**
*  @brief
*    Check if a string matches the given regular expression
*/
bool RegEx::Match(const String &sSubject, uint32 nPosition)
{
	if (m_pPCRE) {
		// Clear results of last operation
		ClearResults();

		// Use ASCII or UTF8 encoding?
		if (m_nFlags & EncodingUTF8) {
			// Match pattern
			int nMatches[VecSize];
			int nMatchCount = pcre_exec(
								m_pPCRE,					// PCRE expression
								m_pExtra,					// Extra data
								sSubject.GetUTF8(),			// Subject string
								sSubject.GetNumOfBytes(),	// Subject length in bytes
								nPosition,					// Start at byte position
								0,							// Default options
								nMatches,					// Output vector with offsets in bytes
								VecSize						// Size of output vector
							  );

			// Output vector too small?
			if (nMatchCount == 0) {
				// Allocate big enough output vector and retry
				// [TODO]
				nMatchCount = MaxGroups;
			}

			// Check result
			if (nMatchCount > 0) {
				// Save new byte byte position within the string
				m_nPosition = nMatches[1];

				// Get pointer to subject string
				const char *pszSubject = sSubject.GetUTF8();

				// Save matching substrings by index
				m_lstGroups.Resize(nMatchCount-1);
				for (int i=1; i<nMatchCount; i++)
					m_lstGroups[i-1].Copy(&pszSubject[nMatches[i*2]], nMatches[i*2+1] - nMatches[i*2]);

				// Get named groups
				int nNameCount;
				pcre_fullinfo(m_pPCRE, m_pExtra, PCRE_INFO_NAMECOUNT, &nNameCount);
				const char *pszNameTable;
				pcre_fullinfo(m_pPCRE, m_pExtra, PCRE_INFO_NAMETABLE, &pszNameTable);
				int nNameEntrySize;
				pcre_fullinfo(m_pPCRE, m_pExtra, PCRE_INFO_NAMEENTRYSIZE, &nNameEntrySize);

				// Return matching substrings by name
				for (int i=0; i<nNameCount; i++) {
					// Get pointer to current entry
					const char *pszEntry = &pszNameTable[i*nNameEntrySize];

					// Get number
					const int nNum = (pszEntry[0] << 8) | pszEntry[1];

					// Get name
					const String sName = String::FromUTF8(&pszEntry[2]);

					// Get substring
					const int nIndex0 = nMatches[nNum*2];
					const int nIndex1 = nMatches[nNum*2+1];

					// Add name->substring to map
					if (nIndex0 >= 0 && nIndex1 >= 0)
						m_mapGroups.Add(sName, String::FromUTF8(&pszSubject[nIndex0], 0, nIndex1 - nIndex0));
					else
						m_mapGroups.Add(sName, "");
				}

				// Done
				return true;
			}
		} else {
			// Match pattern
			int nMatches[VecSize];
			int nMatchCount = pcre_exec(
								m_pPCRE,					// PCRE expression
								m_pExtra,					// Extra data
								sSubject.GetASCII(),		// Subject string
								sSubject.GetNumOfBytes(),	// Subject length in bytes
								nPosition,					// Start at byte position
								0,							// Default options
								nMatches,					// Output vector with offsets in bytes
								VecSize						// Size of output vector
							  );

			// Output vector too small?
			if (nMatchCount == 0) {
				// Allocate big enough output vector and retry
				// [TODO]
				nMatchCount = MaxGroups;
			}

			// Check result
			if (nMatchCount > 0) {
				// Save new byte position within the string
				m_nPosition = nMatches[1];

				// Get pointer to subject string
				const char *pszSubject = sSubject.GetASCII();

				// Save matching substrings by index
				m_lstGroups.Resize(nMatchCount-1);
				for (int i=1; i<nMatchCount; i++)
					m_lstGroups[i-1].Copy(&pszSubject[nMatches[i*2]], nMatches[i*2+1] - nMatches[i*2]);

				// Get named groups
				int nNameCount;
				pcre_fullinfo(m_pPCRE, m_pExtra, PCRE_INFO_NAMECOUNT, &nNameCount);
				const char *pszNameTable;
				pcre_fullinfo(m_pPCRE, m_pExtra, PCRE_INFO_NAMETABLE, &pszNameTable);
				int nNameEntrySize;
				pcre_fullinfo(m_pPCRE, m_pExtra, PCRE_INFO_NAMEENTRYSIZE, &nNameEntrySize);

				// Return matching substrings by name
				for (int i=0; i<nNameCount; i++) {
					// Get pointer to current entry
					const char *pszEntry = &pszNameTable[i*nNameEntrySize];

					// Get number
					const int nNum = (pszEntry[0] << 8) | pszEntry[1];

					// Get name
					const String sName = &pszEntry[2];

					// Get substring
					const int nIndex0 = nMatches[nNum*2];
					const int nIndex1 = nMatches[nNum*2+1];

					// Add name->substring to map
					if (nIndex0 >= 0 && nIndex1 >= 0)
						m_mapGroups.Add(sName, String(&pszSubject[nIndex0], true, nIndex1 - nIndex0));
					else
						m_mapGroups.Add(sName, "");
				}

				// Done
				return true;
			}
		}
	}

	// Error!
	return false;
}

/**
*  @brief
*    Get current byte position in the subject string
*/
int RegEx::GetPosition() const
{
	return m_nPosition;
}

/**
*  @brief
*    Get the results of the last check
*/
const Array<String> &RegEx::GetResults() const
{
	return m_lstGroups;
}

/**
*  @brief
*    Get a matching group from the last check
*/
String RegEx::GetResult(uint32 nIndex) const
{
	return m_lstGroups[nIndex];
}

/**
*  @brief
*    Get the results of the last check by name
*/
const Map<String, String> &RegEx::GetNameResults() const
{
	return m_mapGroups;
}

/**
*  @brief
*    Get a matching group from the last check by name
*/
String RegEx::GetNameResult(const String &sName) const
{
	return m_mapGroups.Get(sName);
}

/**
*  @brief
*    Clear all results
*/
void RegEx::ClearResults()
{
	// Clear lists
	m_lstGroups.Reset();
	m_mapGroups.Clear();

	// Reset byte position
	m_nPosition = 0;
}


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Compiles the PCRE expression
*/
void RegEx::CompilePCRE()
{
	// Free former expression
	FreePCRE();

	// Set options
	const int nOptions = (IsCaseSensitive() ? 0 : PCRE_CASELESS) | (IsGreedy() ? 0 : PCRE_UNGREEDY);

	// Use ASCII or UTF8 encoding?
	if (m_nFlags & EncodingUTF8) {
		const char *pszError;
		int nErrorOffset;

		// Compile regular expression
		m_pPCRE = pcre_compile(m_sExpression.GetUTF8(), nOptions | PCRE_UTF8, &pszError, &nErrorOffset, nullptr);
	} else {
		const char *pszError;
		int nErrorOffset;

		// Compile regular expression
		m_pPCRE = pcre_compile(m_sExpression.GetASCII(), nOptions, &pszError, &nErrorOffset, nullptr);
	}
	if (!m_pPCRE) {
		// Error!
	}
}

/**
*  @brief
*    Deletes the PCRE expression
*/
void RegEx::FreePCRE()
{
	// Free PCRE expression
	if (m_pPCRE) {
		pcre_free(m_pPCRE);
		m_pPCRE  = nullptr;
		m_pExtra = nullptr;
	}
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLGeneral
