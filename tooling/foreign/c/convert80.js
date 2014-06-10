// Converts a VC5 or VC6 DSP Project File to VC7.1 format.
// Invoke with CScript so you don't get UI. 
// Example: cscript convert71.js e:\yourprojects\project.dsp
// Written by: mato@imatix.com

var vcProj  = new ActiveXObject ("VisualStudio.VCProjectEngine.8.0");
var vcSoln  = new ActiveXObject ("VisualStudio.Solution.8.0");
var objFile = new ActiveXObject ("Scripting.FileSystemObject");
var objArgs = WScript.Arguments;

// Check command line arguments
if (objArgs.Count () < 1)
{
   WScript.Echo ("VC6 or 5 DSP Project File Conversion");
   WScript.Echo ("Opens specified .dsp and converts to VC8.0 Format.");
   WScript.Echo ("Will create project file with .vcproj extension and add it");
   WScript.Echo ("to empty solution with .sln extension.");
   WScript.Echo ("\n\tusage: convert80.js project.dsp");
   WScript.Quit (1);
}

// Determine full path of input file
var inputProjFile    = objFile.GetAbsolutePathName (objArgs.Item (0));
var outputBaseName   = objFile.GetBaseName         (inputProjFile);
var outputFolderName = objFile.GetParentFolderName (inputProjFile);
var outputProjFile   = outputFolderName + "\\" + outputBaseName + ".vcproj";
var outputSolnFile   = outputFolderName + "\\" + outputBaseName + ".sln";

// If the output project or solution file already exist, do not convert
if (objFile.FileExists(outputProjFile))
{
   WScript.Echo ("E: " + outputProjFile + " already exists");
   WScript.Quit (1);
}
if (objFile.FileExists (outputSolnFile))
{
   WScript.Echo ("E: " + outputSolnFile + " already exists");
   WScript.Quit (1);
}

WScript.Echo ("I: Converting: " + inputProjFile);

// Load project file into Visual Studio and convert it
var vcProject         = vcProj.LoadProject (inputProjFile);
vcProject.ProjectFile = outputProjFile;
vcProject.Save ();

// Create an empty solution, and add the project to it
vcSoln.Create      (outputFolderName, outputBaseName);
vcSoln.AddFromFile (outputProjFile);
