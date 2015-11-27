********************coding standards**************
# Indenting and Whitespace
Use an indent of 2 spaces, with no tabs.

Lines should have no trailing whitespace at the end.

Files should be formatted with \n as the line ending (Unix line endings), not \r\n (Windows line endings).

All text files should end in a single newline (\n). This avoids the verbose "\ No newline at end of file" patch warning and makes patches easier to read since it's clearer what is being changed when lines are added to the end of a file.


# Operators
All binary operators (operators that come between two values), such as +, -, =, !=, ==, >, etc. should have a space before and after the operator, for readability. For example, an assignment should be formatted as $foo = $bar; rather than $foo=$bar;. Unary operators (operators that operate on only one value), such as ++, should not have a space between the operator and the variable or number they are operating on.

# Casting
Put a space between the (type) and the $variable in a cast: (int) $mynumber.


# Control Structures
Control structures include if, for, while, switch, etc. Here is a sample if statement, since it is the most complicated of them:

if (condition1 || condition2) {
  action1;
}
elseif (condition3 && condition4) {
  action2;
}
else {
  defaultaction;
}

(Note: Don't use "else if" -- always use elseif.)
Control statements should have one space between the control keyword and opening parenthesis, to distinguish them from function calls.

Always use curly braces even in situations where they are technically optional. Having them increases readability and decreases the likelihood of logic errors being introduced when new lines are added. The opening curly should be on the same line as the opening statement, preceded by one space. The closing curly should be on a line by itself and indented to the same level as the opening statement.

For switch statements:

switch (condition) {
  case 1:
    action1;
    break;

  case 2:
    action2;
    break;

  default:
    defaultaction;
}
For do-while statements:

do {
  actions;
} while ($condition);


# Function Calls
Functions should be called with no spaces between the function name, the opening parenthesis, and the first parameter; spaces between commas and each parameter, and no space between the last parameter, the closing parenthesis, and the semicolon. Here's an example:

$var = foo($bar, $baz, $quux);
As displayed above, there should be one space on either side of an equals sign used to assign the return value of a function to a variable. In the case of a block of related assignments, more space may be inserted to promote readability:

$short         = foo($bar);
$long_variable = foo($baz);


 Function Declarations
function funstuff_system($field) {
  $system["description"] = t("This module inserts funny text into posts randomly.");
  return $system[$field];
}
Arguments with default values go at the end of the argument list. Always attempt to return a meaningful value from a function if one is appropriate.


# Arrays
Arrays should be formatted with a space separating each element (after the comma), and spaces around the => key association operator, if applicable:

$some_array = array('hello', 'world', 'foo' => 'bar');

# Quotes
Drupal does not have a hard standard for the use of single quotes vs. double quotes. Where possible, keep consistency within each module, and respect the personal style of other developers.

# String Concatenations
Always use a space between the dot and the concatenated parts to improve readability.


# String Concatenations
Always use a space between the dot and the concatenated parts to improve readability.
