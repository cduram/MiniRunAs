// miniRunAs  -  A minimalist "run as" for Windows (a runas.exe alternative)
//
// Home page: www.source-code.biz/snippets/c/1.htm
// License: GNU/LGPL (www.gnu.org/licenses/lgpl.html)
// Copyright 2008 Christian d'Heureuse, Inventec Informatik AG, Switzerland.
// This software is provided "as is" without warranty of any kind.
//
// Version history:
// 2008-03-09 Christian d'Heureuse (chdh@inventec.ch)
//   Module created.
// 2011-07-05 Christian d'Heureuse (chdh@inventec.ch)
//   lpDomain parameter of CreateProcessWithLogonW changed from L"." to NULL.


#define UNICODE
#define _WIN32_WINNT 0x0500                                // Win2K and later
#include <stdio.h>
#include <Windows.h>

static const char*           programVersion = "2008-03-09";

static wchar_t               user[64];
static wchar_t               password[64];
static wchar_t               commandLine[1024];

static void displayHelp() {
   printf ("\n");
   printf ("miniRunAs  -  A minimalist \"run as\"\n");
   printf ("\n");
   printf ("Usage:   miniRunAs <user> <password> <commandline>\n");
   printf ("Example: miniRunAs administrator sesame ping localhost\n");
   printf ("Author:  Christian d'Heureuse, www.source-code.biz, chdh@inventec.ch\n");
   printf ("License: GNU/LGPL (www.gnu.org/licenses/lgpl.html)\n");
   printf ("Version: %s\n", programVersion); }

static void displayWin32ApiError (const char* routineName) {
   DWORD errorCode = GetLastError();
   wchar_t* msg = NULL;
   int i = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL, errorCode, 0, (LPWSTR)&msg, 0, NULL);
   if (i == 0) {
      fprintf (stderr, "FormatMessage failed for error code %i.\n", errorCode); return; }
   fwprintf (stderr, L"Error code %i returned from %S.\n%s\n", errorCode, routineName, msg);
   LocalFree (msg); }

static const wchar_t* skipBlanks (const wchar_t* s) {
   while (*s == L' ') s++;
   return s; }

static const wchar_t* skipNonBlanks (const wchar_t* s) {
   while (*s != 0 && *s != L' ') s++;
   return s; }

static const wchar_t* skipPastChar (const wchar_t* s, wchar_t c) {
   while (*s != 0) {
      if (*s == c) {s++; break; }
      s++; }
   return s; }

static const wchar_t* parseNextWord (const wchar_t* s, wchar_t* wBuf, int wBufSize) {
   const wchar_t* s1 = skipBlanks(s);
   const wchar_t* s2 = skipNonBlanks(s1);
   wcsncpy_s (wBuf, wBufSize, s1, s2-s1);
   return s2; }

static bool parseCommandLineParms() {
   const wchar_t* s = GetCommandLine();
   s = skipBlanks(s);
   if (*s == L'"')
      s = skipPastChar(s+1, L'"');                         // if the commandline starts with a quote, we have to skip past the next quote
    else
      s = skipNonBlanks(s);                                // otherwise the program path does not contain blanks and we skip to the next blank
   s = skipBlanks(s);
   if (*s == 0) {                                          // no command-line parameters
      displayHelp();
      return false; }
   s = parseNextWord(s, user, sizeof(user)/2);
   s = parseNextWord(s, password, sizeof(password)/2);
   s = skipBlanks(s);
   wcscpy_s (commandLine, s);
   if (commandLine[0] == 0) {
      fprintf (stderr, "Missing command-line arguments.\n");
      return false; }
   return true; }

int main() {
   if (!parseCommandLineParms()) return 9;
   STARTUPINFOW si;
   memset (&si, 0, sizeof(si));
   si.cb = sizeof(si);
   PROCESS_INFORMATION pi;
   BOOL ok = CreateProcessWithLogonW (
      user,
      NULL,                                                // change to L"." to use local account database only
      password,
      LOGON_WITH_PROFILE,
      NULL,
      commandLine,
      0,
      NULL,
      NULL,
      &si,
      &pi);
   if (ok == 0) {
      displayWin32ApiError ("CreateProcessWithLogonW");
      return 9; }
   return 0; }