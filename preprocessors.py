import re

from nbconvert.preprocessors import Preprocessor

ANS_ASSIGMENT_REGEX = re.compile(r'=\s*[^\s].*#\s+\?\?\?\s*$', flags=re.MULTILINE)
ANS_CALL_REGEX = re.compile(r'(\(.+\))\s*#\s+\?\?\?\s*$', flags=re.MULTILINE)

class FilterTutorialCells(Preprocessor):
    def preprocess(self, notebook, resources):
        executable_cells = []
        for cell in notebook.cells:
            if cell['cell_type'] == 'code':
                cell['source'] = re.sub(
                    ANS_ASSIGMENT_REGEX,
                    "= ???",
                    cell['source'],
                )
                cell['source'] = re.sub(
                    ANS_CALL_REGEX,
                    "(???)",
                    cell['source'],
                )
                metadata = cell.setdefault('metadata', {})
                if "execution" in metadata:
                    del metadata["execution"]
                
            executable_cells.append(cell)
        notebook.cells = executable_cells
        return notebook, resources
