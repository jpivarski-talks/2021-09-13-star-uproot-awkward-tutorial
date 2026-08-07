#!/bin/sh

exec jupyter nbconvert \
	--Exporter.preprocessors=nbconvert.preprocessors.ExecutePreprocessor \
	--allow-errors \
	--Exporter.preprocessors=preprocessors.FilterTutorialCells \
	tutorial.ipynb --to notebook --output tutorial_export.ipynb
