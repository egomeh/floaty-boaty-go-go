import os
import sys
import Tkinter, tkFileDialog

headerTemplate = """#pragma once

#include "logic.hpp"
#include "util.hpp"

class {0} : public LogicComponent
{{
public:
    {0}()
    {{
    }}

    void Update() override;
    void Start() override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {{
    }}

private:
}};
"""

sourceTemplate = """
#include "{0}.hpp"

void {0}::Update()
{{
}}

void {0}::Start()
{{
}}
"""

def main():
	initialDir = os.path.dirname(__file__) + "/../assets"
	print initialDir

	newFilename = tkFileDialog.asksaveasfilename(initialdir = initialDir, title = "New logic component")

	headerFilename = newFilename + ".hpp"
	sourceFilename = newFilename + ".cpp"

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
