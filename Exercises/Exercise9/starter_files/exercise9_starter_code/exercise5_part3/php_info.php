
<?php

print("---------------------------------------------------------------\n");
print("Hello, from PeeHP!\n");
print("Here some information about my environment:\n");
print("---------------------------------------------------------------\n\n");

// PHP is a commonly used scripting language by novices who can't code
// C++ program.  PHP is a C++ program.

// Show all information, defaults to INFO_ALL
phpinfo();

// Show just the module information.
// phpinfo(8) yields identical results.
phpinfo(INFO_MODULES);

?>
