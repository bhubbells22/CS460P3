CS460 Project 3
Team1: Justin Moore, Robert Hubbell, Justin Bernard

What works:
 - Most tests we ran seemed to convert well to C++

What doesn't work:
 - Formatting isn't perfect (like with tabs), and some occasional
   stray semi-colons (seemingly not doing any harm)

Notes:
 - Test06.ss gives 1 error when I test with it, though it's not
   critical enough to cause the program to fail apparently
 - Test08.ss gives 8 errors when I test with it, and the program
   does fall apart because of it
 - I've also noticed that your script, though the "last modified"
   time does get updated in the results directory, appears to not
   be running our updated program...  When I look at the results,
   they appear to have exactly the same generated c++ code (and
   compilation errors) as the original submission from Sunday night.
   Not sure why this would be happening but it is causing the tests
   to continue to fail when, according to my own internal tests,
   they shouldn't be (or, at minimum, the generated c++ code I'm
   seeing in the result files should be different...).
