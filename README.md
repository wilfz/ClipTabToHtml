# ClipTabToHtml
Have you ever tried to add Tab-separated data (e.g. output from SQL Server Management Studio) to a mail?
It looks awful!

Here comes this litte tool:
just copy the content to the clipboeard, run ClipTabToHtml.exe and paste into your email body, that's it.

What does the program do?
It just parses the tab separated data from the clipboard, builds an html table from it, converts all to UTF-8 and embeds all into a construct called HTML fragment.
This HTML fragment is added to the clipboard as an additional format.

Since nowadays most email is formatted as HTML you get a table that actually looks like a table.
(This little program does not an ned not have a gui and works only on windows.)
