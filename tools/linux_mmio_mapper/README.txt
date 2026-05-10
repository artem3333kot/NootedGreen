Linux-First MMIO Mapper

Files:
- mapping.schema.json: strict schema contract for model outputs.
- validate_mappings.py: validates policy and writes safe AUTO_RENAME subset.
- sample_mapping.json: example using ICL_PWR_WELL_CTL_DDI2.

Usage:
1) Validate and extract safe renames:
   python3 validate_mappings.py sample_mapping.json --approved-out approved_auto_renames.json

2) Non-zero exit means your model output violated policy.

3) Generate Linux-name-first header from approved mappings:
   python3 generate_header_from_approved.py approved_auto_renames.json --out linux_mmio_aliases.h

Policy enforced:
- Linux symbol is canonical when available.
- UNKNOWN_0x... cannot be AUTO_RENAME.
- AUTO_RENAME requires exact address match + platform match + HIGH confidence + no ambiguity.
- Confidence must match score band:
  HIGH >= 10, MEDIUM 7..9, LOW <= 6.
