# Sirius-Shell
<p>Sirius Shell provides an interface to the Unix system. It gathers input from the user and executes programs based on that input. When a program finishes executing, it displays that program's output.
Shell is an environment in which we can run our commands, programs, and shell scripts.
</p>
<p>
It does three main things in its lifetime, they are as follows:
<ul>
<li><b>Initialize:</b> In this step, a typical shell would read and execute its configuration files. These change aspects of the shell’s behavior.</li>
<li><b>Interpret:</b> Next, the shell reads commands from stdin (which could be interactive, or a file) and executes them.</li>
<li><b>Terminate:</b> After it's commands are executed, the shell executes any shutdown commands, frees up any memory, and terminates.</li>
</ul>
</p>
<p>
Sirius Shell handles commands with three steps:
<ul>
<li><b>Read:</b> Read the command from standard input.</li>
<li><b>Parse:</b> Separate the command string into a program and arguments.</li>
<li><b>Execute:</b> Run the parsed command.</li>
</ul>
</p>
