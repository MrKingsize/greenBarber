# Aromatics Recognizer

This folder holds the code for the Aromatics Recognizer. The Aromatics Recognizer is a component of the Green Barber Robot that is responsible for recognizing aromatic plants, i.e., decide whether the plant is, or is not, fit to be harvested.

# Requirements
- Python 3.12.4 or higher;
- Pip 24.1.2 or higher;
- Dependencies listed in the `requirements.txt` file.

# Setup

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

# Usage

First, make sure that you have files (aromatic_n.png and notaromatic_n.png) in the ./train folder.
Second, run the trainer.ipynb notebook to train the model.
Third, evaluate results.
Fourth, use the model to predict new images using recognizer.ipynb notebook.