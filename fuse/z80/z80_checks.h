SETUP_CHECK( profile, profile_active )
SETUP_CHECK( rzx, rzx_playback )
SETUP_CHECK( debugger, debugger_mode != DEBUGGER_MODE_INACTIVE )
SETUP_CHECK( beta, beta_available )
SETUP_CHECK( plusd, plusd_available )
SETUP_CHECK( disciple, disciple_available )
SETUP_CHECK( if1p, if1_available )
SETUP_CHECK( divide_early, settings_current.divide_enabled )
SETUP_CHECK( spectranet_page, spectranet_available && !settings_current.spectranet_disable )
SETUP_NEXT( opcode_delay )
SETUP_CHECK( evenm1, even_m1 )
SETUP_NEXT( run_opcode )
SETUP_CHECK( if1u, if1_available )
SETUP_CHECK( divide_late, settings_current.divide_enabled )
SETUP_CHECK( opus, opus_available )
SETUP_CHECK( spectranet_unpage, spectranet_available )
SETUP_CHECK( rpage, rfsh_paging )
SETUP_NEXT( end_opcode )
