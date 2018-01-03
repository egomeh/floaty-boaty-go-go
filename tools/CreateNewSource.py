import os
import sys
import Tkinter, tkFileDialog

headerTemplate = """#pragma once

class {0}
{{
public:
    {0}();
    ~{0}();

private:
}};

"""

sourceTemplate = """#include "{0}.hpp"

{0}::{0}()
{{
}}

{0}::~{0}()
{{
}}

"""

def main():
	initialDir = os.path.dirname(__file__) + "/../src"
	print initialDir

	newFilename = tkFileDialog.asksaveasfilename(initialdir = initialDir, title = "New source files")

	headerFilename = newFilename.lower + ".hpp"
	sourceFilename = newFilename.lower + ".cpp"

	if os.path.isfile(headerFilename) or os.path.isfile(sourceFilename):
		return

	basename = os.path.basename(headerFilename).split(".")[0]

	folderName = os.path.dirname(newFilename)

	headerContent = headerTemplate.format(basename)

	sourceContent = sourceTemplate.format(basename)

	with open(headerFilename, "w") as file:
		file.write(headerContent)

	with open(sourceFilename, "w") as file:
		file.write(sourceContent)

if __name__ == "__main__":
	main()
