# VRASED End-To-End Proofs

### Pre-Requisite:

- Install Spot 2.0 (https://spot.lrde.epita.fr/install.html)

### Running the Proofs:

1) chmod +x *.ltl

2) sh proof_theorem1.ltl

3) change "proof_theorem1.ltl" to the proof you want to run

Altenatively, to install Spot 2.0 and compute all 3 proofs run:

        sudo sh run_proofs_end_to_end.sh

### File contents:
- proof_theorem1.ltl contains the proof for Theorem 1 in VRASED paper (https://arxiv.org/abs/1811.00175)
- The proof for Theorem 2 in VRASED's paper is split into two files:
	1) proof_theorem2_1.ltl contains the proof for preventing untrusted software from directly accessing the attestation key.
	2) proof_theorem2_2.ltl contains the proof for no key leakage through memory
- ".comment" files contain step-by-step explanations of each respective proof

