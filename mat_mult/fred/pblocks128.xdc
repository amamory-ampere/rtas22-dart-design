# User Generated miscellaneous constraints


set_property HD.RECONFIGURABLE true [get_cells dart_i/acc_0/inst]
create_pblock pblock_slot_0
add_cells_to_pblock [get_pblocks pblock_slot_0] [get_cells -quiet [list dart_i/acc_0/inst]]
resize_pblock [get_pblocks pblock_slot_0] -add {SLICE_X18Y120:SLICE_X29Y419}
resize_pblock [get_pblocks pblock_slot_0] -add {RAMB18_X3Y48:RAMB18_X3Y167}
resize_pblock [get_pblocks pblock_slot_0] -add {RAMB36_X3Y24:RAMB36_X3Y83}
resize_pblock [get_pblocks pblock_slot_0] -add {DSP48E2_X3Y48:DSP48E2_X5Y167}

#resize_pblock pblock_slot_0 -add {SLICE_X18Y180:SLICE_X55Y419 DSP48E2_X3Y72:DSP48E2_X11Y167 RAMB18_X3Y72:RAMB18_X6Y167 RAMB36_X3Y36:RAMB36_X6Y83} -remove {SLICE_X18Y180:SLICE_X29Y419 DSP48E2_X3Y72:DSP48E2_X5Y167 RAMB18_X3Y72:RAMB18_X3Y167 RAMB36_X3Y36:RAMB36_X3Y83} -locs keep_all

resize_pblock pblock_slot_0 -add {SLICE_X0Y180:SLICE_X96Y419 DSP48E2_X0Y72:DSP48E2_X17Y167 IOB_X0Y156:IOB_X0Y279 RAMB18_X0Y72:RAMB18_X12Y167 RAMB36_X0Y36:RAMB36_X12Y83} -remove {SLICE_X18Y180:SLICE_X96Y419 DSP48E2_X3Y72:DSP48E2_X17Y167 IOB_X0Y156:IOB_X0Y279 RAMB18_X3Y72:RAMB18_X12Y167 RAMB36_X3Y36:RAMB36_X12Y83} -locs keep_all


set_property RESET_AFTER_RECONFIG true [get_pblocks pblock_slot_0]
set_property SNAPPING_MODE ON [get_pblocks pblock_slot_0]


set_property SEVERITY {Warning} [get_drc_checks NSTD-1]
set_property SEVERITY {Warning} [get_drc_checks UCIO-1]
