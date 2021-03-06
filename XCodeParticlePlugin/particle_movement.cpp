#define EXPORT_API
#include <cmath>
#include <iostream>

extern "C"
{
/*    const EXPORT_API void centralGravity(float* particle_states,float* center,float dt)
    {
        float* x=&particle_states[0];
        float* v=&particle_states[3];
        
        float gravity[3]; //"gravity" here is just a direction from the particle's position to the specified center
        for(int d=0;d<3;d++){
            gravity[d]=center[d]-x[d];
        }

        float magnitude=std::sqrt(gravity[0]*gravity[0] + gravity[1]*gravity[1] + gravity[2]*gravity[2]);
        float epsilon=1e-10f;

        for(int d=0;d<3;d++){
            gravity[d]/=(magnitude+epsilon);
        }
        
        for(int d=0;d<3;d++){
            v[d]+=gravity[d]*dt;
        }
        for(int d=0;d<3;d++){
            x[d]+=v[d]*dt;
        }
    }*/
    
    const EXPORT_API void physicsStep(float* particle_states, float* all_particles_data, int particles_count, float* gravity, float* bounds, float damping_percentage, int attractors_count, float* attractor_data, float dt, bool bounds_is_circle)
    {
        float* x=&particle_states[0];
        float* v=&particle_states[3];
        float a[3]={0, 0, 0};
        
        //attractos will have:
        
        //float gravity[3] = {0, -1.0f, 0};
        //float bounds[6] = {-10.0f, 0.0f, -10.0f, 10.0f, 20.0f, 10.0f};
        
        // /* calculating forces */
        // attractions from attractors
        for (int i = 0; i < attractors_count; i++) {
            float distance_squared = 0.0f;
            float distance[3];
            for(int d=0;d<3;d++){
                distance[d] = x[d] - attractor_data[4 * i + d];
                distance_squared += distance[d] * distance[d];
            }
            //distance_cubed *= std::sqrt(distance_cubed);
            for(int d=0;d<3;d++){
                a[d] += attractor_data[4 * i + 3] * distance[d] / distance_squared;
            }
        }
        
        // repulsions between particles
        for (int i = 0; i < particles_count; i++) {
            float distance_squared = 0.0f;
            float distance[3];
            for(int d=0;d<3;d++){
                distance[d] = x[d] - all_particles_data[4 * i + d];
                distance_squared += distance[d] * distance[d];
            }
            if (distance_squared < 0.001f) {
                continue;
            }
            for(int d=0;d<3;d++){
                a[d] -= all_particles_data[4 * i + 3] * distance[d] / distance_squared;
            }
        }
        
        // gravity
        for(int d=0;d<3;d++){
            a[d] += gravity[d];
        }
        // /* DONE calculating forces */
        
        for(int d=0;d<3;d++){
            v[d] += a[d] * dt;
            x[d] += v[d] * dt;
        }
        
        if (bounds_is_circle) {
            
            float* bounds_circle = &bounds[0];
            
            float circle_x = bounds_circle[0];
            float circle_y = bounds_circle[1];
            float circle_z = bounds_circle[2];
            float circle_r = bounds_circle[3];
            
            float dist_X = x[0] - circle_x;
            float dist_Y = x[1] - circle_y;
            float dist_Z = x[2] - circle_z;
            
            float r_2_after = dist_X * dist_X + dist_Y * dist_Y + dist_Z * dist_Z;
            
            if (r_2_after > circle_r * circle_r) {
                float force_magnitude = (2 - damping_percentage) * (v[0] * dist_X + v[1] * dist_Y + v[2] * dist_Z) / (circle_r * circle_r);
                
                v[0] -= force_magnitude * dist_X;
                v[1] -= force_magnitude * dist_Y;
                v[2] -= force_magnitude * dist_Z;
                
                float ratio = circle_r / std::sqrt(r_2_after);
                x[0] = circle_x + dist_X * ratio;
                x[1] = circle_y + dist_Y * ratio;
                x[2] = circle_z + dist_Z * ratio;
            }

        } else {
            
            float* bounds_low = &bounds[0];
            float* bounds_high = &bounds[3];
            
            for (int d = 0; d < 3; d++) {
                if (x[d] < bounds_low[d]) {
                    v[d] = -v[d] * (1.0f - damping_percentage);
                    x[d] += 2 * (bounds_low[d] - x[d]);
                }
                if (x[d] > bounds_high[d]) {
                    v[d] = -v[d] * (1.0f - damping_percentage);
                    x[d] += 2 * (bounds_high[d] - x[d]);
                }
            }
        }
        
        
        
        
    }
    
} // end of export C block
