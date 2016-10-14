<?xml version="1.0" encoding="UTF-8"?>
<GestureMarkupLanguage xmlns:gml="http://gestureworks.com/gml/version/1.0" mouse_simulator="false">

	<Gesture_set gesture_set_name="basic-gestures">
			
		<Gesture id="n-drag" type="drag">
			<comment>The 'n-drag' gesture can be activated by any number of touch points. When a touch down is recognized on a touch object the position
			of the touch point is tracked. This change in the position of the touch point is mapped directly to the position of the touch object.</comment>
			<match>
				<action>
					<initial>
						<cluster point_number="0" point_number_min="1" point_number_max="10"/>
					</initial>
				</action>
			</match>	
			<analysis>
				<algorithm class="kinemetric" type="continuous">
					<library module="drag"/>
					<returns>
						<property id="drag_dx" result="dx"/>
						<property id="drag_dy" result="dy"/>
					</returns>
				</algorithm>
			</analysis>	
			<processing>
				<inertial_filter>
					<property ref="drag_dx" active="true" friction="0.9"/>
					<property ref="drag_dy" active="true" friction="0.9"/>
				</inertial_filter>
				<delta_filter>
					<property ref="drag_dx" active="true" delta_min="0.05" delta_max="500"/>
					<property ref="drag_dy" active="true" delta_min="0.05" delta_max="500"/>
				</delta_filter>
			</processing>
			<mapping>
				<update dispatch_type="continuous">
					<gesture_event type="drag">
						<property ref="drag_dx" target="x"/>
						<property ref="drag_dy" target="y"/>
					</gesture_event>
				</update>
			</mapping>
		</Gesture>
			
		<Gesture id="1-drag" type="drag">
			<comment>The 'n-drag' gesture can be activated by any number of touch points. When a touch down is recognized on a touch object the position
			of the touch point is tracked. This change in the position of the touch point is mapped directly to the position of the touch object.</comment>
			<match>
				<action>
					<initial>
						<cluster point_number="1"/>
					</initial>
				</action>
			</match>	
			<analysis>
				<algorithm class="kinemetric" type="continuous">
					<library module="drag"/>
					<returns>
						<property id="drag_dx" result="dx"/>
						<property id="drag_dy" result="dy"/>
					</returns>
				</algorithm>
			</analysis>	
			<mapping>
				<update dispatch_type="continuous">
					<gesture_event type="drag">
						<property ref="drag_dx" target="x"/>
						<property ref="drag_dy" target="y"/>
					</gesture_event>
				</update>
			</mapping>
		</Gesture>
		
		<Gesture id="n-rotate-and-scale" type="rotate-and-scale">
		    <match>
		        <action>
		            <initial>
		                <cluster point_number="0" point_number_min="1" point_number_max="10"/>
		            </initial>
		        </action>
		    </match>       
		    <analysis>
		        <algorithm class="kinemetric" type="continuous">
		            <library module="rotate-and-scale"/>
		            <returns>
		                <property id="rotate_and_scale_dx" result="dx"/>
		                <property id="rotate_and_scale_dy" result="dy"/>
		                <property id="rotate_and_scale_dsx" result="ds"/>
		                <property id="rotate_and_scale_dsy" result="ds"/>
		                <property id="rotate_and_scale_dtheta" result="dtheta"/>
		            </returns>
		        </algorithm>
		    </analysis>    
		    <mapping>
		        <update dispatch_type="continuous">
		            <gesture_event type="rotate-and-scale">
		                <property ref="rotate_and_scale_dx" target="x"/>
		                <property ref="rotate_and_scale_dy" target="y"/>
		                <property ref="rotate_and_scale_dsx" target="scaleX"/>
		                <property ref="rotate_and_scale_dsy" target="scaleY"/>
		                <property ref="rotate_and_scale_dtheta" target="rotation"/>
		            </gesture_event>
		        </update>
		    </mapping>
		</Gesture>
			
		<Gesture id="n-rotate" type="rotate">
			<match>
				<action>
					<initial>
						<cluster point_number="0" point_number_min="2" point_number_max="10"/>
					</initial>
				</action>
			</match>
			<analysis>
				<algorithm class="kinemetric" type="continuous">
					<library module="rotate"/>
					<returns>
						<property id="rotate_dtheta" result="dtheta"/>
					</returns>
				</algorithm>
			</analysis>	
			<processing>
				<inertial_filter>
					<property ref="rotate_dtheta" active="true" friction="0.9"/>
				</inertial_filter>
				<delta_filter>
					<property ref="rotate_dtheta" active="true" delta_min="0.01" delta_max="20"/>
				</delta_filter>
			</processing>
			<mapping>
				<update dispatch_type="continuous">
					<gesture_event type="rotate">
						<property ref="rotate_dtheta" target="rotate"/>
					</gesture_event>
				</update>
			</mapping>
		</Gesture>
		
		<Gesture id="n-scale" type="scale">
			<match>
				<action>
					<initial>
						<cluster point_number="0" point_number_min="2" point_number_max="10"/>
					</initial>
				</action>
			</match>
			<analysis>
				<algorithm class="kinemetric" type="continuous">
					<library module="scale"/>
					<returns>
						<property id="scale_dsx" result="ds"/>
						<property id="scale_dsy" result="ds"/>
					</returns>
				</algorithm>
			</analysis>	
			<processing>
				<inertial_filter>
					<property ref="scale_dsx" active="true" friction="0.9"/>
					<property ref="scale_dsy" active="true" friction="0.9"/>
				</inertial_filter>
				<delta_filter>
					<property ref="scale_dsx" active="false" delta_min="0.0001" delta_max="1"/>
					<property ref="scale_dsy" active="false" delta_min="0.0001" delta_max="1"/>
				</delta_filter>
				<multiply_filter>
					<property ref="scale_dsx" active="true" func="linear" factor="0.0033"/>
					<property ref="scale_dsy" active="true" func="linear" factor="0.0033"/>
				</multiply_filter>
			</processing>
			<mapping>
				<update dispatch_type="continuous">
					<gesture_event type="scale">
						<property ref="scale_dsx" target="scaleX"/>
						<property ref="scale_dsy" target="scaleY"/>
					</gesture_event>
				</update>
			</mapping>
		</Gesture>	
		
		<Gesture id="n-scroll" type="scroll">
			<comment>The 'n-scroll' gesture can be activated by any number of touch points. When a touch down is recognized on a touch object, 
			the average velocity of the touch points are tracked. If velocity of the cluster is above the translation threshold a scroll event 
			is continuously dispatched with inertia.</comment>
			
			<match>
				<action>
					<initial>
						<cluster point_number="0" point_number_min="1" point_number_max="5"/>
					</initial>
				</action>
			</match>
			<analysis>
				<algorithm class="kinemetric" type="continuous">
					<library module="scroll"/>
					<variables>
						<property id="scroll_dx" return="etm_dx" var="etm_dx" var_min="1"/>
						<property id="scroll_dy" return="etm_dy" var="etm_dy" var_min="1"/>
					</variables>
					<returns>
						<property id="scroll_dx" result="etm_dx"/>
						<property id="scroll_dy" result="etm_dy"/>
					</returns>
				</algorithm>
			</analysis>	
			<processing>
				<inertial_filter>
					<property ref="scroll_dx" active="true" friction="0.94"/>
					<property ref="scroll_dy" active="true" friction="0.94"/>
				</inertial_filter>
				<delta_filter>
					<property ref="scroll_dx" active="true" delta_min="0.5" delta_max="30"/>
					<property ref="scroll_dy" active="true" delta_min="0.5" delta_max="30"/>
				</delta_filter>
			</processing>
			<mapping>
				<update dispatch_type="continuous">
					<gesture_event  type="scroll">
						<property ref="scroll_dx" target=""/>
						<property ref="scroll_dy" target=""/>
					</gesture_event>
				</update>
			</mapping>
		</Gesture>

							
		
	</Gesture_set>
	
	<Gesture_set gesture_set_name="conditional-gestures">
	
			<Gesture id="n-swipe" type="swipe">
				<comment>The 'n-swipe' gesture can be activated by any number of touch points. When a touch down is recognized on a touch object, the velocity and 
				acceleration of the touch points are tracked. If acceleration of the cluster is below the acceleration threshold a swipe event is dispatched.</comment>
				<match>
					<action>
						<initial>
							<cluster point_number="0" point_number_min="1" point_number_max="5"/>
							<event touch_event="touchEnd"/>
						</initial>
					</action>
				</match>
				<analysis>
					<algorithm class="kinemetric" type="continuous">
						<library module="swipe" />
						
						<variables>
							<property id="swipe_dx" var="etm_ddx" return="etm_dx" var_max="10"/>
							<property id="swipe_dy" var="etm_ddy" return="etm_dy" var_max="10"/>
						</variables>
						<returns>
							<property id="swipe_dx" result="etm_dx"/>
							<property id="swipe_dy" result="etm_dy"/>
						</returns>
					</algorithm>
				</analysis>	
				<mapping>
					<update dispatch_type="discrete" dispatch_mode="cluster_remove" dispatch_reset="cluster_remove">
						<gesture_event  type="swipe">
							<property ref="swipe_dx" target=""/>
							<property ref="swipe_dy" target=""/>
						</gesture_event>
					</update>
				</mapping>
			</Gesture>	
	
			<Gesture id="n-flick" type="flick">
				<comment>The 'n-flick' gesture can be activated by any number of touch points. When a touch down is recognized on a touch object, the velocity and 
				acceleration of the touch points are tracked. If acceleration of the cluster is above the acceleration threshold a flick event is dispatched.</comment>
			<match>
				<action>
					<initial>
						<cluster point_number="0" point_number_min="1" point_number_max="5" acceleration_min="0.5"/>
						<event touch_event="touchEnd"/>
					</initial>
				</action>
			</match>
			<analysis>
				<algorithm class="kinemetric" type="continuous">
					<library module="flick"/>
					<variables>
						<property id="flick_dx" var="etm_ddx" return="etm_dx" var_min="10"/>
						<property id="flick_dy" var="etm_ddy" return="etm_dy" var_min="10"/>
					</variables>
					<returns>
						<property id="flick_dx" result="etm_dx"/>
						<property id="flick_dy" result="etm_dy"/>
					</returns>
				</algorithm>
			</analysis>	
			<mapping>
				<update dispatch_type="discrete" dispatch_mode="cluster_remove" dispatch_reset="cluster_remove">
					<gesture_event  type="flick">
						<property ref="flick_dx" target=""/>
						<property ref="flick_dy" target=""/>
					</gesture_event>
				</update>
			</mapping>
		</Gesture>
		
	</Gesture_set>
	
	<Gesture_set gesture_set_name="temporal-gestures">
	
		<Gesture id="n-tap" type="tap">
			<match>
				<action>
					<initial>
						<point event_duration_max="200" translation_max="10"/>
						<cluster point_number="0"/>
						<event touch_event="touchEnd"/>
					</initial>
				</action>
			</match>	
			<analysis>
				<algorithm class="temporalmetric" type="discrete">
					<library module="tap"/>
					<returns>
						<property id="tap_x" result="x"/>
						<property id="tap_y" result="y"/>
						<property id="tap_n" result="n"/>
					</returns>
				</algorithm>
			</analysis>	
			<mapping>
				<update dispatch_type="discrete" dispatch_mode="batch" dispatch_interval="0">
					<gesture_event  type="tap">
						<property ref="tap_x"/>
						<property ref="tap_y"/>
						<property ref="tap_n"/>
					</gesture_event>
				</update>
			</mapping>
		</Gesture>
		
		<Gesture id="n-double_tap" type="double_tap">
			<match>
				<action>
					<initial>
						<point event_duration_max="300" interevent_duration_max="300" translation_max="20"/>
						<cluster point_number="0"/>
						<event gesture_event="tap"/>
					</initial>
				</action>
			</match>	
			<analysis>
				<algorithm class="temporalmetric" type="discrete">
					<library module="double_tap"/>
					<returns>
						<property id="double_tap_x" result="x"/>
						<property id="double_tap_y" result="y"/>
						<property id="double_tap_n" result="n"/>
					</returns>
				</algorithm>
			</analysis>	
			<mapping>
				<update dispatch_type="discrete" dispatch_mode="batch" dispatch_interval="0">
					<gesture_event  type="double_tap">
						<property ref="double_tap_x"/>
						<property ref="double_tap_y"/>
						<property ref="double_tap_n"/>
					</gesture_event>
				</update>
			</mapping>
		</Gesture>
		
		<Gesture id="n-hold" type="hold">
			<match>
				<action>
					<initial>
						<point event_duration_min="500" translation_max="2"/>
						<cluster point_number="0" point_number_min="1" point_number_max="5"/>
					</initial>
				</action>
			</match>	
			<analysis>
				<algorithm class="temporalmetric" type="discrete">
					<library module="hold"/>
					<returns>
						<property id="hold_x" result="x"/>
						<property id="hold_y" result="y"/>
						<property id="hold_n" result="n"/>
					</returns>
				</algorithm>
			</analysis>	
			<mapping>
				<update dispatch_type="discrete" dispatch_reset="cluster_remove">
					<gesture_event  type="hold">
						<property ref="hold_x"/>
						<property ref="hold_y"/>
						<property ref="hold_n"/>
					</gesture_event>
				</update>
			</mapping>
		</Gesture>
		
	</Gesture_set>
	
</GestureMarkupLanguage>
