// A simple JavaScript to create input boxes and run commands typed-in
// by the user.

// This is a counter to track the currently active command input from
// the user.
var cmdID = 1;

function enterKey(event) {
    // Check and run command if the enter key is pressed.
    if (event.keyCode === 13) {
        // Cancel the default action, if needed
        event.preventDefault();
        // Disable the input
        event.target.disabled = true;
        // Run the command specified by the user.
        run(event.target.value);
    }
}

function createInput() {
    // Add new command-input to the HTML.
    var div = document.createElement('div');
    div.innerHTML = "$<input id='" + cmdID + "'></div>";
    document.body.appendChild(div);
    // Setup listener to handle the enter key press in this input.
    var input = document.getElementById(cmdID);
    input.addEventListener("keyup", enterKey);
    input.focus();
    // Add control to handle 
    cmdID++;
}

function run(cmd) {
    if (cmd.length > 0) {
        // Run the command using an AJAX request.
        var xhttp = new XMLHttpRequest();
        // Setup handler to add result to the HTML
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                var output = document.createElement('pre');
                output.innerHTML = this.responseText;
                document.body.appendChild(output);
            }
        }
        // Run the command.
        console.log("Running command: " + cmd);
        cmd = encodeURIComponent(cmd);
        xhttp.open("GET", "/cgi-bin/exec?cmd=" + cmd, false);
        xhttp.send();
    }
    // Create input box for user to enter the next command to run.
    createInput();
}
